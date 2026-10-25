document.querySelectorAll(".play-button").forEach(button => {
    button.addEventListener("click", async () => {
        const movieId = button.dataset.movieId;

        button.disabled = true;
        button.textContent = "Iniciando...";

        try {
            const response = await fetch(
                `/api/movies/${movieId}/play`,
                { method: "POST" }
            );

            const result = await response.json();

            if (!response.ok) {
                throw new Error(
                    result.error || "Falha ao iniciar o filme."
                );
            }

            window.location.href = `/player/${movieId}`;

        } catch (error) {
            alert(error.message);
            button.disabled = false;
            button.textContent = "▶ Reproduzir";
        }
    });
});