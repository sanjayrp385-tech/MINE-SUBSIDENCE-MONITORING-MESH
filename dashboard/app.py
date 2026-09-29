from flask import Flask, jsonify, render_template
import serial, threading, time, os

app=Flask(__name__)
PORT=os.environ.get("MESH_SERIAL_PORT","/dev/ttyACM0")
BAUD=115200
nodes={}
lock=threading.Lock()

def risk(tx,ty,v,sw):
    tilt=max(abs(tx),abs(ty))
    # DEMO thresholds only; not mine-safety limits.
    if sw==1 or tilt>=20 or v>=0.20: return "HIGH"
    if tilt>=10 or v>=0.10: return "WARNING"
    return "NORMAL"

def parse(packet):
    vals={}
    for p in packet.split(","):
        if "=" in p:
            k,v=p.split("=",1); vals[k.strip()]=v.strip()
    if "N" not in vals: return
    n=int(vals["N"])
    tx=float(vals.get("TX",0)); ty=float(vals.get("TY",0))
    vib=float(vals.get("V",0)); sw=int(vals.get("SW",0))
    with lock:
        nodes[n]={"node":n,"tilt_x":round(tx,2),"tilt_y":round(ty,2),
                  "vibration":round(vib,3),"switch":sw,
                  "risk":risk(tx,ty,vib,sw),"timestamp":time.time()}

def serial_reader():
    while True:
        try:
            print("Opening",PORT)
            with serial.Serial(PORT,BAUD,timeout=1) as ser:
                print("Connected to",PORT)
                while True:
                    line=ser.readline().decode("utf-8",errors="ignore").strip()
                    if line:
                        print(line)
                        if line.startswith("DATA|"): parse(line[5:])
        except Exception as e:
            print("Serial error:",e)
            time.sleep(2)

@app.route("/")
def index(): return render_template("dashboard.html")

@app.route("/api/data")
def data():
    now=time.time()
    with lock:
        out={}
        for n,d in nodes.items():
            x=dict(d); x["online"]=(now-d["timestamp"])<10
            out[str(n)]=x
        return jsonify(out)

@app.route("/api/health")
def health():
    return jsonify({"gateway":"online","serial_port":PORT,"nodes_received":len(nodes)})

if __name__=="__main__":
    threading.Thread(target=serial_reader,daemon=True).start()
    print("MINE SUBSIDENCE MONITORING")
    print("Dashboard: http://0.0.0.0:5000")
    app.run(host="0.0.0.0",port=5000,debug=False)
