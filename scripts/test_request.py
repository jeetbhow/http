#!/usr/bin/env python3
"""Send a test HTTP request to the local TCP server."""

import argparse
import socket
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8080)
    args = parser.parse_args()

    request = (
        "GET / HTTP/1.1\r\n"
        f"Host: {args.host}:{args.port}\r\n"
        "User-Agent: http-test-client\r\n"
        "Connection: close\r\n"
        "\r\n"
    ).encode("ascii")

    try:
        with socket.create_connection((args.host, args.port), timeout=5) as client:
            client.sendall(request)
            # The server reads until EOF; signal it without closing the receive side.
            client.shutdown(socket.SHUT_WR)
            while data := client.recv(4096):
                sys.stdout.buffer.write(data)
    except OSError as error:
        print(f"Request failed: {error}", file=sys.stderr)
        return 1

    print(f"Sent GET / to {args.host}:{args.port}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
