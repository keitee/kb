# app/routes.py, Home page route

from app import app

@app.route('/')
@app.route('/index')

def index():
    return "Hello, Flash!"
