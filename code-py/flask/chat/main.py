# https://codeburst.io/building-your-first-chat-application-using-flask-in-7-minutes-f98de4adfa5d
# Building your first Chat Application using Flask in 7 minutes

from flask import Flask, render_template
from flask_socketio import SocketIO

# What is SocketIO?
# SocketIO is a cross-browser Javascript library that abstracts the client
# application from the actual transport protocol. For modern browsers the
# WebSocket protocol is used, but for older browsers that don't have WebSocket
# SocketIO emulates the connection using one of the older solutions, the best
# one available for each given client.

# SocketIO is a cross-browser Javascript library that abstracts the client
# application from the actual transport protocol. Using SocketIO, you can be
# pretty much sure that any browser out there will be able to connect to your
# application.

# Now, create a variable named app by wrapping Flask around __name__. To enable
# encryption, we’re declaring a SECRET_KEY.

# SocketIO is being applied to ‘app’ and is later being stored in socketio
# variable which enables us to use socketio instead of app in running the
# application. socketio encapsulates startup of the web server, i.e. app.

# The run() method takes optional host and port arguments, but by default it
# will listen on localhost:5000 like Flask’s development web server. debug=True
# enables to sort out the errors with ease.

app = Flask(__name__)
app.config['SECRET_KEY'] = 'vnkdjnfjknfl1232#'
sio = SocketIO(app)

# Flask Templates helps in adding HTML to our application. These must be
# included in templates folder. The hierarchy is as follows, assuming the folder
# or the directory we’re creating this app in is Chat_room.
#
# Chat_room folder
# /main.py
# templates
#        |-/session.html
#
# session.html:
# 
# <html>
# <head>
#     <title>Session</title>
# </head>
# <body>
#     <p>Hello</p>
# </body>
# </html>

@app.route('/')
def sessions():
    return render_template('session.html')

# To receive WebSocket messages from the client, the application defines event
# handlers using the socketio.on decorator and it can send reply messages to the
# connected client using the send() and emit() functions.

# On triggering ‘my event’, the ‘handle_my_custom_event’ view firstly receives
# the json objects, prints it on the terminal and later sends it to the ‘my
# response’ event. ‘callback’ is kind of debugging technique which helps us in
# getting to know if the message is received by the server and henceforth, calls
# the function ‘messageReceived’.

def messageReceived(methods=['GET', 'POST']):
    print("CHAT: message was received")

@sio.on('my event')
def handle_my_custom_event(json, methods=['GET', 'POST']):
    print("CHAT: received my event: " + str(json))
    sio.emit('my response', json, callback=messageReceived)


# Integrating SocketJS in Flask Templates
#
# It’s time to fill in session.html with socket code.
#
# Firstly, we need to include jquery and socket.js scripts in our HTML file. To
# do that we need to import these scripts before writing our JS Functions.
#
# <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js"></script>
# <script src="https://cdnjs.cloudflare.com/ajax/libs/socket.io/1.7.3/socket.io.min.js"></script>

# Secondly, we need to prompt both the users( involved in the chat ) to enter
# the message that needs to be delivered for which we need to create a form in
# HTML. This should be connected to socket.js by using ‘form’ element and
# classes of the two input fields as well. The below code snippet explains the
# creation of a form:

# <form action="" method="POST">
#       <input type="text" class="username" placeholder="User Name"/>
#       <input type="text" class="message" placeholder="Messages"/>
#       <input type="submit"/>
# </form>

# Next, we need to use io.connect() for establishing the connections and create
# sessions, the session will be created by connecting both the users to the same
# URL which is http://127.0.0.1:5000.
#
# Here, document.domain represents the IP address of the computer you are
# working on and location.port represents the port, default one is 5000 and if
# passed as an argument in the run function, it would be that.
#
# var socket = io.connect('http://' + document.domain + ':' + location.port);

# The first argument to socket.on() is the event name. Event names ‘connect’,
# ‘disconnect’, ‘message’ and ‘json’ are special events generated by SocketIO.
# Any other event names are considered custom events.

# The ‘connect’ and ‘disconnect’ events are self-explanatory. The ‘message’
# event delivers a payload of type string, and the ‘json’ and custom events
# deliver a JSON payload, in the form of a Python dictionary.

# To send events, a Flask server can use the send() and emit() functions
# provided by Flask-SocketIO. The send() function sends a standard message of
# string or JSON type to the client. The emit() function sends a message under a
# custom event name( in here it’s ‘my event’ ) along with the data.

# On submitting the form, i.e. after invoking the POST method, an event ‘e’ is
# passed as an argument where preventDefault() method is called which prevents
# from submitting the form and later, takes in the values from input fields,
# username and message. These are then passed through the ‘emit’ function to the
# custom event ‘my event’ which we earlier defined in ‘main.py’. Thereafter, the
# message field is cleared to enable the same user to enter more messages.

# socket.on( 'my response', function( msg ) {
#   console.log( msg )
#   if( typeof msg.user_name !== 'undefined' ) {
#     $( 'h3' ).remove()
#     $( 'div.message_holder' ).append('<div><b style="color: #000">'+msg.user_name+'</b>'+msg.message+'</div>' )
#   }
# })

# Once the message is sent through an event, we need to render the received
# message on HTML page. On response, as soon as the message is received we pass
# in the message to “message_holder” class( along with which text in h3 tag(“No
# message yet..”) is removed ), where our messages are appended to the previous
# ones.


# It’s not just confined to two persons but indeed allows a group of people. For
# instance, open another tab and type in your messages. Voila! You could group
# text too.

# By this, you successfully mastered in building a chat application using flask.


# <!DOCTYPE html>
# <html lang="en">
# <head>
#   <title>Flask_Chat_App</title>
# </head>
# <body>

#   <h3 style='color: #ccc;font-size: 30px;'>No message yet..</h3>
#   <div class="message_holder"></div>

#   <form action="" method="POST">
#     <input type="text" class="username" placeholder="User Name"/>
#     <input type="text" class="message" placeholder="Messages"/>
#     <input type="submit"/>
#   </form>

#   <!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->
#   <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js"></script>
#   <script src="https://cdnjs.cloudflare.com/ajax/libs/socket.io/1.7.3/socket.io.min.js"></script>
#   <script type="text/javascript">
#     var socket = io.connect('http://' + document.domain + ':' + location.port);
#     socket.on( 'connect', function() {
#       socket.emit( 'my event', {
#         data: 'User Connected'
#       } )
#       var form = $( 'form' ).on( 'submit', function( e ) {
#         e.preventDefault()
#         let user_name = $( 'input.username' ).val()
#         let user_input = $( 'input.message' ).val()
#         socket.emit( 'my event', {
#           user_name : user_name,
#           message : user_input
#         } )
#         $( 'input.message' ).val( '' ).focus()
#       } )
#     } )
#     socket.on( 'my response', function( msg ) {
#       console.log( msg )
#       if( typeof msg.user_name !== 'undefined' ) {
#         $( 'h3' ).remove()
#         $( 'div.message_holder' ).append( '<div><b style="color: #000">'+msg.user_name+'</b> '+msg.message+'</div>' )
#       }
#     })
#   </script>

# </body>
# </html>

# main

if __name__ == '__main__':
    sio.run(app, debug=True)


