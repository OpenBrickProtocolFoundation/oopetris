FROM  python:3.11

ADD . /service

WORKDIR /service

RUN pip install -U meson ninja

RUN meson setup build

RUN meson compile -C build

EXPOSE 8000 8000

ENTRYPOINT ["./build/oopetris_backend"]
