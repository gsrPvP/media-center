import json
import socket
import struct

SOCKET_PATH = "/tmp/media-center.sock"

CMD_PLAY = 0

CMD_GET_STATUS = 7

COMMANDS = {
    "pause": 1,
    "stop": 2,
    "fwd": 3,
    "bwd": 4,
    "seek": 5,
    "resume": 6,
}

def play_movie(path):
    # O C espera o caminho terminado em '\0'.
    payload = path.encode("utf-8") + b"\0"

    # CommandHeader: unsigned int type + unsigned int data_len.
    header = struct.pack("=II", CMD_PLAY, len(payload))

    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as sock:
        sock.connect(SOCKET_PATH)
        sock.sendall(header)
        sock.sendall(payload)


def get_status():
    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as sock:
        sock.settimeout(3)
        sock.connect(SOCKET_PATH)

        # Header: tipo do comando + tamanho do payload
        header = struct.pack("=II", CMD_GET_STATUS, 0)
        sock.sendall(header)

        # Lê a resposta até o delimitador de linha enviado pelo C
        response = bytearray()

        while b"\n" not in response:
            chunk = sock.recv(1024)

            if not chunk:
                raise ConnectionError(
                    "O servidor C fechou a conexão sem responder."
                )

            response.extend(chunk)

        # O servidor envia JSON terminado em '\n'
        line = bytes(response).split(b"\n", 1)[0]
        return json.loads(line.decode("utf-8"))

def send_command(command, seconds=None):
    if command not in COMMANDS:
        raise ValueError("Comando Invalido")

    payload = b""

    if command == "seek":
        if seconds == None:
            raise ValueError("O comando seek exige seconds.")

        seconds = int(seconds)

        if  seconds < 0:
            raise ValueError("A posição não pode ser negativa")

        payload = str(seconds).encode("utf-8") + b"\0"

    header = struct.pack("=II", COMMANDS[command], len(payload))

    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as sock:
        sock.settimeout(3)
        sock.connect(SOCKET_PATH)
        sock.sendall(header)

        if payload:
            sock.sendall(payload)