#!/usr/bin/python3

import requests
from http.server import HTTPServer, BaseHTTPRequestHandler

class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        string = self.headers['interval'] + " " + \
                self.headers['delay'] + " " +     \
                self.headers['repeat'] + "\n"
        with open('/proc/oneshot', 'w') as f:
            f.write(string)
        print(f"Start oneshot: {string}", end='')
        self.end_headers()

print('wait for args from guest')
server = HTTPServer(('192.168.122.1', 8887), RequestHandler)
server.handle_request() # wait for guest init params

print('reading oneshot for result')
with open('/proc/oneshot', 'rb') as f:
    res = f.read(8)
    steps = int.from_bytes(res[0:4], byteorder='little')
    energy = int.from_bytes(res[4:8], byteorder='little')

print(f"Got result steps = {steps}, energy = {energy}")

headers = {
    "steps": str(steps),
    "energy": str(energy),
}
# Done
print(f'Try http to guest with results {headers}')
requests.get("http://192.168.122.15:8888", headers=headers)
