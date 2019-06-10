# https://medium.com/python-pandemonium/build-simple-restful-api-with-python-and-flask-part-1-fae9ff66a706
# Build Simple Restful Api With Python and Flask Part 1

from flask import Flask

app = Flask(__name__)

@app.route('/')
def hello():
    return 'Hello, Build Simple Restful Api With Python and Flask Part 1'

# To run, $python app.py
if __name__ == '__main__':
    app.run(debug=True)
