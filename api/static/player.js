const message = document.getElementById("player-message");
const progress = document.getElementById("movie-progress");
const currentTime = document.getElementById("current-time");
const totalTime = document.getElementById("total-time");
const playerStatus = document.getElementById("player-status");
const pauseButton = document.querySelector(".pause-button");

let currentState = "stopped";
let draggingProgress = false;

function formatTime(seconds) {
    seconds = Math.max(0, Math.floor(Number(seconds) || 0));

    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const remainingSeconds = seconds % 60;

    if (hours > 0) {
        return `${hours}:${String(minutes).padStart(2, "0")}:${String(remainingSeconds).padStart(2, "0")}`;
    }

    return `${minutes}:${String(remainingSeconds).padStart(2, "0")}`;
}

async function sendCommand(command, seconds = null) {
    const body = { command };

    if (command === "seek") {
        body.seconds = Number(seconds);
    }

    const response = await fetch("/api/player/command", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify(body)
    });

    const result = await response.json();

    if (!response.ok || !result.success) {
        throw new Error(result.error || "Falha ao enviar comando.");
    }

    message.textContent = `Comando enviado: ${command}`;
    await updatePlayerStatus();
}

async function updatePlayerStatus() {
    try {
        const response = await fetch("/api/player/status", {
            cache: "no-store"
        });

        if (!response.ok) {
            throw new Error("Não foi possível consultar o player.");
        }

        const status = await response.json();

        currentState = status.state;

        currentTime.textContent = formatTime(status.position);
        totalTime.textContent = formatTime(status.duration);

        progress.max = Math.max(1, Number(status.duration) || 1);

        if (!draggingProgress) {
            progress.value = status.position;
        }

        if (playerStatus) {
            playerStatus.textContent = status.state.toUpperCase();
        }

        if (pauseButton) {
            const isPlaying = currentState === "playing";

            pauseButton.textContent = isPlaying ? "Ⅱ" : "▶";
            pauseButton.title = isPlaying ? "Pausar" : "Retomar";
        }
    } catch (error) {
        console.error("Erro ao atualizar player:", error);
    }
}

document.querySelectorAll("[data-action]").forEach(button => {
    button.addEventListener("click", async () => {
        const action = button.dataset.action;

        try {
            if (action === "pause") {
                const command = currentState === "playing"
                    ? "pause"
                    : "resume";

                await sendCommand(command);
            } else if (action === "fwd" || action === "bwd") {
                await sendCommand(action);
            } else if (action === "stop") {
                const confirmed = confirm(
                    "Deseja parar de assistir e voltar ao catálogo?"
                );

                if (!confirmed) return;

                button.disabled = true;

                try {
                    await sendCommand("stop");
                    window.location.href = "/movies";
                } catch (error) {
                    button.disabled = false;
                    throw error;
                }
            }
        } catch (error) {
            message.textContent = `Erro: ${error.message}`;
            console.error("Erro ao executar comando:", error);
        }
    });
});

progress.addEventListener("input", () => {
    draggingProgress = true;
    currentTime.textContent = formatTime(progress.value);
});

progress.addEventListener("change", async () => {
    try {
        await sendCommand("seek", progress.value);
    } catch (error) {
        message.textContent = `Erro ao buscar posição: ${error.message}`;
        console.error(error);
    } finally {
        draggingProgress = false;
        await updatePlayerStatus();
    }
});

updatePlayerStatus();
setInterval(updatePlayerStatus, 1000);