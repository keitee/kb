# Build Simple Restful Api With Python and Flask Part 2

# On this part we import all module that needed by our application. We import
# Flask to create instance of web application, request to get request data,
# jsonify to turns the JSON output into a `Response` object with the
# application/json mimetype, SQAlchemy from flask_sqlalchemy to accessing
# database, and Marshmallow from flask_marshmallow to serialized object.

from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from flask_marshmallow import Marshmallow
import os

# This part create an instances of our web application and set path of our
# SQLite uri.

app = Flask(__name__)
basedir = os.path.abspath(os.path.dirname(__file__))
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + os.path.join(basedir, 'crud.sqlite')

# On this part we binding SQLAlchemy and Marshmallow into our flask application.

db = SQLAlchemy(app)
ma = Marshmallow(app)


# After import SQLAlchemy and bind it to our flask app, we can declare our
# models. Here we declare model called User and defined its field with it’s
# properties.

class User(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    username = db.Column(db.String(80), unique=True)
    email = db.Column(db.String(120), unique=True)

    def __init__(self, username, email):
        self.username = username
        self.email = email


# This part defined structure of response of our endpoint. We want that all of
# our endpoint will have JSON response. Here we define that our JSON response
# will have two keys(username, and email). Also we defined user_schema as
# instance of UserSchema, and user_schemas as instances of list of UserSchema.

class UserSchema(ma.Schema):
    class Meta:
        # fields to expose
        fields = ('username', 'email')

user_schema = UserSchema()
users_schema = UserSchema(many=True)


# For short code above will have 5 endpoints with capabilities to create new
# record, get all records from database, get record detail by id, update
# selected record, and delete selected record. Also on this code we define model
# for our database.

# On this part we define endpoint to create new user. First we set the route to
# “/user” and set HTTP methods to POST. After set the route and methods we
# define function that will executed if we access this endpoint. On this
# function first we get username and email from request data. After that we
# create new user using data from request data. Last we add new user to data
# base and show new user in JSON form as `response`.

# `endpoint` to create new user
@app.route("/user", methods=['POST'])
def add_user():
    print('post.create user')
    # username = request.json['username']
    # email = request.json['email']

    # new_user = User(username, email)

    # db.session.add(new_user)
    # db.session.commit()

    # return jsonify(new_user)
    return "received /user POST"


# On this part we define endpoint to get list of all users and show the result
# as JSON response.

# endpoint to show all users
@app.route("/user", methods=['GET'])
def get_user():
    print('get.user')
    # all_users = User.query.all()
    # result = users_schema.dump(all_users)
    # return jsonify(result.data)
    return "received /user GET"


# Like on previous part on this part we define endpoint to get user data, but
# instead of get all the user here we just get data from one user based on id.
# If you look carefully at the route, you can see different pattern on the route
# of this endpoint. Patern like “<id>” is parameter, so you can change it with
# everything you want. This parameter should put on function parameter(in this
# case def user_detail(id)) so we can get this parameter value inside function.

# endpoint to get user detail by id
@app.route("/user/<id>", methods=['GET'])
def user_detail(id):
    print('get.user.%s detail' % str(id))
    # user = User.query.get(id)
    # return user_schema.jsonify(user)

    return "received /user/" + str(id) + " GET"

# custom endpoint to have more arguments
# curl -v -X GET http://localhost:5000/user/200/kit/park

# keitee@keitee-mint:~/mw/ethan-as-source-repo$ curl -v -X GET http://localhost:5000/user/200/kit/park
# Note: Unnecessary use of -X or --request, GET is already inferred.
# *   Trying 127.0.0.1...
# * TCP_NODELAY set
# * Connected to localhost (127.0.0.1) port 5000 (#0)
# > GET /user/200/kit/park HTTP/1.1
# > Host: localhost:5000
# > User-Agent: curl/7.58.0
# > Accept: */*
# >
# * HTTP 1.0, assume close after body
# < HTTP/1.0 200 OK
# < Content-Type: text/html; charset=utf-8
# < Content-Length: 38
# < Server: Werkzeug/0.15.4 Python/3.7.3
# < Date: Wed, 12 Jun 2019 08:14:27 GMT
# <
# * Closing connection 0
# received /user/200 name: kit, park GETkeitee@keitee-mint:~/mw/ethan-as-source-repo$

@app.route("/user/<id>/<givenname>/<surname>", methods=['GET'])
def user_custom(id, givenname, surname):
    print('get.user.%s.%s.%s detail' % (str(id), givenname, surname))
    # user = User.query.get(id)
    # return user_schema.jsonify(user)

    return "received /user/" + str(id) + " name: " + givenname + ", " + surname +  " GET"

# On this part we define endpoint to update user. First we call user that
# related with given id on parameter. Then we update username and email value of
# this user with value "from request data."

# endpoint to update user
@app.route("/user/<id>", methods=['PUT'])
def user_update(id):
    print('put.user update')
    user = User.query.get(id)

    username = request.json['username']
    email = request.json['email']

    user.username = username
    user.email = email

    db.session.commit()
    return user_schema.jsonify(user)


# Last we define endpoint to delete user. First we call user that related with
# given id on parameter. Then we delete it.

# endpoint to delete user
@app.route("/user/<id>", methods=['DELETE'])
def user_delete(id):
    print('delete.user')
    user = User.query.get(id)
    db.session.delete(user)
    db.session.commit()
    return user_schema.jsonify(user)

# c. Generate SQLite database
#
# In the previous step you have write code to handle CRUD operation to SQLite,
# but if you run this python file and try to access the endpoint(you can try to
# access localhost:5000/user) you will get the error message similar with

# OperationalError: (sqlite3.OperationalError) no such table: user [SQL:
# u’SELECT user.id AS user_id, user.username AS user_username, user.email AS
# user_email \nFROM user’]

# This error message occurred because you trying to get data from SQLite, but
# you don’t have SQLite database yet. So In this step we will generate SQLite
# database first before running our application. You can generate SQLite
# database based on your model in crud.py using following step.

# enter into python interactive shell
#
# First you need to enter into python interactive shell using following command
# in your terminal:
#
# $ python
#
# 2. import db object and generate SQLite database
#
# Use following code in python interactive shell
#
# >>> from crud import db
# >>> db.create_all()
#
# And "crud.sqlite" will be generated inside your flask_tutorial folder.

# d. Run flask application
#
# Now after generate sqlite database, we are ready to run our flask application.
# Run following command from your terminal to run your application.
#
# $ python crud.py

if __name__ == '__main__':
    app.run(debug=True)
