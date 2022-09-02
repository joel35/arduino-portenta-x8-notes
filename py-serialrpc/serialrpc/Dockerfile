FROM python:alpine

LABEL maintainer="Giampaolo Mancini <g.mancini@arduino.cc>"

WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY src/ .

ENTRYPOINT [ "python", "serialrpc.py" ]
