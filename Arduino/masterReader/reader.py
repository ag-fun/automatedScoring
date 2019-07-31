import time
from serial import Serial
import requests

arduino = Serial("COM5", 115200, timeout=0.1)
time.sleep(1)  # give the connection a second to settle
# arduino.write("Hello from Python!")
def postScore(field, score_a, score_b):
	data = {"team_a": score_a, "team_b": score_b, "field": field}
	r= requests.post(
		"https://poolimate.arsludendi.org/api/sendScore",
		# "http://localhost:5000/api/sendScore",
		json=data,
		headers={"X-POOLAUTH": "42"},
	)
	print(r.text)

while True:
		data = arduino.readline()
		if data:
			testdata = data.decode("utf-8", "replace")
			split = testdata.split(",")
			print(split)
			score_a = split[2].split(":")[1]
			score_b = split[3].split(":")[1]
			print(score_a)
			print(score_b)
			postScore("1", score_a, score_b)

        # postScore()
        # print(data)
        # (better to do .read() in the long run for this reason


