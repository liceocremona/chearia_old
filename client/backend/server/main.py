import os
from flask import Flask
from flask import request
from flask import redirect
from flask import jsonify
from flask_cors import CORS
import jwt
import hashlib
import requests
from pytz import timezone
from datetime import datetime
import re
from dotenv import load_dotenv

#load key from env variable
load_dotenv()
KEY = bytes(os.getenv("KEY"), "utf-8")
FIREBASE_TOKEN = os.getenv("FIREBASE_TOKEN")
STORAGE= os.getenv("STORAGE")

#init Flask app
app = Flask(__name__)
CORS(app,
origins=["https://progettochearia.it",
 "https://web.progettochearia.it",
 "https://admin.progettochearia.it"]
)

#regex code and timezone
timestamp_regex = re.compile("^(0[1-9]|[12][0-9]|3[01])[- \/.](0[1-9]|1[012])[- \/.](19|20)\d\d$") #https://www.regular-expressions.info/dates.html
dataid_regex = re.compile("(example|CO|altitude|humidity|ozone|pressure|temperature)")
rome_tz = timezone('Europe/Rome')

def checkLogin(user_dict):
    req = requests.get("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db3-user.json?auth="+ FIREBASE_TOKEN)
    db = req.json()
    username_s = user_dict["username"]
    pass_hash = hashlib.sha256(str.encode(user_dict["password"])).hexdigest()
    for user in db:
        if username_s==user["username"] and pass_hash==user["password"]:
            return user_dict

def generateToken(user_dict):
    user_dict = checkLogin(user_dict)
    if user_dict:
        return jwt.encode(user_dict, KEY, algorithm="HS256")

def checkToken(dict_token):
    user_dict = jwt.decode(dict_token, KEY, algorithms="HS256")
    if user_dict:
        user_dict = checkLogin(user_dict)
        if user_dict:
            return user_dict

@app.route('/', methods = ['GET'])
def get():
    return "ciao"

@app.route('/liststorage/bytype', methods=['POST'])
def liststoragebytype():
    body = request.json

    datas_id = ["example"]
    timestamps = ["27-04-2022"]
    returngraph = []
    for root, dirs, files in os.walk(STORAGE):
        for file in files:
            if file.endswith(".svg"):
                graph = file.replace(STORAGE, "")
                graph_name = graph.replace(".svg", "")
                data_id = graph_name.split("_")[-1]
                if data_id in datas_id:
                    data_timestamp = graph_name.split("_")[-2]
                    name = graph_name.split("_")[-3]
                    if data_timestamp in timestamps:
                        returngraph.append({
                            "file": graph,
                            "title": name,
                        })
    return jsonify(returngraph)

@app.route('/liststorage', methods=['GET'])
def liststorage():
    args_date_list = request.args.getlist("date")
    
    if not len(args_date_list) > 0:
        return jsonify({"error": "date required"}), 500
  
    for date_timestamp in args_date_list:
        if not timestamp_regex.match(date_timestamp):
            return jsonify({"error": "invalid date"}), 500
        
    if request.args.getlist("id") != None:
        datas_id = request.args.getlist("id")
    else:
        datas_id = []
    timestamps = args_date_list
    returngraph = []

    for root, dirs, files in os.walk(STORAGE):
        for file in files:
            if file.endswith(".svg"):
                graph = file.replace(STORAGE, "")
                graph_name = graph.replace(".svg", "")
                data_timestamp = graph_name.split("_")[-1]
                if data_timestamp in timestamps:
                    if len(datas_id) == 0:
                        name = graph_name.split("_")[-3]
                        returngraph.append({
                            "file": graph,
                            "title": name,
                        })
                        continue
                    else:
                        data_id = graph_name.split("_")[-2]
                        
                        if data_id in datas_id:
                            name = graph_name.split("_")[-3]
                            returngraph.append({
                                "file": graph,
                                "title": name,
                            })
    if len(returngraph) == 0:
        return jsonify({"error": "no data found"}), 404
    return jsonify(returngraph), 200
                

@app.route('/dbtest.json', methods = ['PUT'])
def getbody():
    data = request.get_data()
    print(data)
    return data

@app.route("/time", methods = ['GET'])
def gettime():
    
    time = datetime.now(rome_tz)
    return time.strftime('%H-%M')

@app.route("/timems/")
@app.route("/timems/<tz>")
def gettimems(tz=None):
    if tz:
        now_timezone = timezone('ETC/GMT'+tz)
    else:
        now_timezone = rome_tz
    
    now = datetime.now(now_timezone)
    midnight = now.replace(hour=0, minute=0, second=0, microsecond=0)
    ms_from_midnight = ((now - midnight).seconds) * 1000
    return str(ms_from_midnight)




@app.route("/time/hour", methods = ['GET'])
def gettime_h():
    time = datetime.now(rome_tz)
    return time.strftime('%H')

@app.route("/time/min", methods = ['GET'])
def gettime_min():
    time = datetime.now(rome_tz)
    return time.strftime('%M')

@app.route("/date", methods = ['GET'])
@app.route("/date/<tz>", methods = ['GET'])
def getdate(tz=None):
    if tz:
        now_timezone = timezone("ETC/GMT"+tz)
    else:
        now_timezone = rome_tz
    time = datetime.now(now_timezone)
    return time.strftime('%d-%m-%Y')


@app.route("/account/signup", methods = ['GET'])
def signup():
    return "non funzionante"
    

@app.route("/account/login", methods = ['GET'])
def login():
    user_dict = {"username": request.args.get("username"), "password": request.args.get("password")}
    token = generateToken(user_dict)
    return redirect("https://admin.progettochearia.it/login_redirect?token="+ token)

@app.route("/account/checktoken", methods = ['GET'])
def checkauth():
    user = checkToken(request.args.get("token"))
    if user:
        return jsonify(user)

@app.route("/private/set_alarm", methods = ['GET'])
def alarm():
    user = checkToken(request.args.get("token"))
    if user:
        allarm_status_req = requests.get("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db4/allarm/status.json?auth="+ FIREBASE_TOKEN)
        if int(allarm_status_req.text) == 0:
            body = 1
        elif int(allarm_status_req.text) == 1:
            body = 0
        set_state = requests.put("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db4/allarm/status.json?auth="+ FIREBASE_TOKEN, data=str(body))
        set_state = requests.patch("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db4/allarm.json?auth="+ FIREBASE_TOKEN, data='{"action":"sicuro"}')
        if set_state.text:
            return set_state.text
    else:
        return "failed"

@app.route("/private/get_alarm", methods = ['GET'])
def get_alarm():
    user = checkToken(request.args.get("token"))
    if user:
        allarm_status_req = requests.get("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db4/allarm/status.json?auth="+ FIREBASE_TOKEN)
        return allarm_status_req.text
    else:
        return "failed"

@app.route("/private/get_alarm_stat", methods = ['GET'])
def get_alarm_state():
    user = checkToken(request.args.get("token"))
    if user:
        allarm_status_req = requests.get("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db4/allarm/action.json?auth="+ FIREBASE_TOKEN)
        return allarm_status_req.text
    else:
        return "failed"

@app.route("/private/set_balloon", methods = ['GET'])
def set_ballon():
    user = checkToken(request.args.get("token"))
    if user:
        balloon_status = requests.get("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db4/pallone/stato.json")
        ballon_action = requests.get("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db4/pallone/azione.json")
        s = str(balloon_status.text)
        a = str(ballon_action.text)
        if (s == '"terra"'):
            req = requests.patch("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db4/pallone.json?auth="+FIREBASE_TOKEN, data='{"azione":"aria"}')
            return jsonify(req.text)
        elif s == '"aria"':
            req = requests.patch("https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db4/pallone.json?auth="+FIREBASE_TOKEN, data='{"azione":"terra"}')
            return jsonify(req.text)
        else:
            return "failed"
    else:
        return "failed_h"
if __name__ == "__main__":
    app.run()