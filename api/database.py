import sqlite3
from pathlib import Path


BASE_DIR = Path(__file__).resolve().parent.parent
DB_PATH = BASE_DIR / "database" / "media.db"

def get_connection():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


def add_movie(title, year, path, duration_seconds=None):
    conn = get_connection()

    try:
        cursor = conn.execute(
            """
            INSERT INTO movies
                (title, year, path, duration_seconds)
            VALUES (?, ?, ?, ?)
            """,
            (title, year, path, duration_seconds),
        )

        conn.commit()
        return cursor.lastrowid

    finally:
        conn.close()