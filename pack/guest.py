#!/usr/bin/python3

import requests
from http.server import HTTPServer, BaseHTTPRequestHandler
import subprocess
import sys

class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        steps = int(self.headers['steps'])
        energy = int(self.headers['energy'])
        print(f"Steps: {steps}, Energy: {energy}, Average: {energy / steps if steps != 0 else 0.0}")
        self.end_headers()

if len(sys.argv) != 5:
    print("Not enough arguments")
    sys.exit(1)

# run victim program
headers = {
    "interval": sys.argv[2],
    "delay":    sys.argv[3],
    "repeat":   sys.argv[4],
}
print(f'Try http to host with {headers}')
requests.get("http://192.168.122.1:8887", headers = headers)
print('fork victim')
sub = subprocess.Popen(["./victim", sys.argv[1]])

# wait analysis
print('wait for result')
server = HTTPServer(('0.0.0.0', 8888), RequestHandler)
server.handle_request()
sub.kill()
