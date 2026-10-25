import struct
from flask import Flask, request, render_template, redirect, url_for
import sqlite3
from database import get_connection, add_movie
from media_client import play_movie, get_status, send_command


app = Flask(__name__)

@app.get("/")
def home():
    return render_template("index.html")

@app.get("/api/movies")
def list_movies():
    conn = get_connection()

    try:
        cursor = conn.execute("""
            SELECT id, title, year, path, duration_seconds
            FROM movies
            ORDER BY title
        """)

        movies = [dict(row) for row in cursor.fetchall()]
        
        return movies
    
    finally:
        conn.close()

@app.get("/api/movies/<int:movie_id>")
def get_movie(movie_id):
    conn = get_connection()

    try:
        cursor = conn.execute(
            """
            SELECT id, title, year, path, duration_seconds
            FROM movies
            WHERE id = ?
            """,
            (movie_id,),
        )
        movie = cursor.fetchone()

        if movie is None:
            return {"error": "Filme não encontrado"}, 404
        return dict(movie)
        
    finally:
        conn.close()

@app.post("/api/movies")
def create_movie():
    data = request.get_json()

    if not data:
        return {"error": "JSON body required"},400

    title = data.get("title")
    year = data.get("year")
    path = data.get("path")
    duration = data.get("duration_seconds")

    if not title or not path:
        return {"error": "title and path are required"},400

    try:
        movie_id = add_movie(
            title=title,
            year=year,
            path=path,
            duration_seconds=duration
        )
    except Exception:
        app.logger.exception("Failed to insert movie")
        return {"error": ""}

    return {
        "id": movie_id,
        "title": title,
        "year": year,
        "path": path,
        "duration_seconds": duration,
    }, 201


## RENDER

@app.get("/movies")
def movies_page():
    conn = get_connection()

    try:
        cursor = conn.execute("""
            SELECT id, title, year, duration_seconds
            FROM movies
            ORDER BY title
        """)

        movies = [dict(row) for row in cursor.fetchall()]

        return render_template(
            "movies.html",
            movies=movies
        )

    finally:
        conn.close()


@app.route("/movies/new", methods=["GET", "POST"])
def new_movie_page():
    if request.method == "GET":
        return render_template("movie_form.html")

    title = request.form.get("title", "").strip()
    year_raw = request.form.get("year", "").strip()
    path = request.form.get("path", "").strip()
    duration_raw = request.form.get("duration_seconds", "").strip()

    # Validar campos obrigatórios
    if not title or not path:
        return render_template(
            "movie_form.html",
            error="Título e caminho do arquivo são obrigatórios."
        ), 400

    # Converter os campos numéricos
    try:
        year = int(year_raw) if year_raw else None
        duration = int(duration_raw) if duration_raw else None

        if year is not None and year <= 0:
            raise ValueError

        if duration is not None and duration <= 0:
            raise ValueError

    except ValueError:
        return render_template(
            "movie_form.html",
            error="Ano e duração devem ser números positivos."
        ), 400

    try:
        add_movie(
            title=title,
            year=year,
            path=path,
            duration_seconds=duration
        )
    except sqlite3.IntegrityError:
        return render_template(
            "movie_form.html",
            error="Esse caminho já está cadastrado ou os dados violam uma regra do banco."
        ), 409

    return redirect(url_for("movies_page"))


@app.post("/api/movies/<int:movie_id>/play")
def play_movie_route(movie_id):
    conn = get_connection()

    try:
        movie = conn.execute(
            "SELECT id, title, path FROM movies WHERE id = ?",
            (movie_id,)
        ).fetchone()
    finally:
        conn.close()

    if movie is None:
        return {"error": "Filme não encontrado"}, 404

    try:
        play_movie(movie["path"])
    except (OSError, struct.error):
        app.logger.exception("Falha ao enviar comando de reprodução")
        return {"error": "Não foi possível comunicar com o player"}, 503

    return {
        "message": "Comando de reprodução enviado",
        "movie_id": movie["id"],
        "title": movie["title"]
    }, 200


##player

@app.get("/player/<int:movie_id>")
def player_page(movie_id):
    conn = get_connection()
    try:
        movie = conn.execute(
            "SELECT * FROM movies WHERE id = ?",
            (movie_id,)
        ).fetchone()
    finally:
        conn.close()

    if movie is None:
        return "Filme não encontrado", 404

    return render_template("player.html", movie=movie)

@app.get("/api/player/status")
def player_status():
    try:
        return get_status()
    except (OSError, ConnectionError, TimeoutError) as error:
        return {"error": str(error)}, 503

@app.post("/api/player/command")
def player_command():
    data = request.get_json(silent=True) or {}
    command = data.get("command")

    if not isinstance(command, str):
        return {"error": "Informe um comando."}, 400

    try:
        if command == "seek":
            seconds = data.get("seconds")

            if isinstance(seconds, bool) or not isinstance(
                seconds, (int, float)
            ):
                return {"error": "Informe seconds como número."}, 400

            if not seconds.is_integer() if isinstance(seconds, float) else False:
                return {"error": "seconds deve ser inteiro."}, 400

            send_command(command, int(seconds))
        else:
            send_command(command)

        return {"success": True, "command": command}

    except ValueError as error:
        return {"error": str(error)}, 400
    except (OSError, ConnectionError, TimeoutError) as error:
        return {"error": str(error)}, 503

if __name__ == "__main__":
    app.run(debug=True)
