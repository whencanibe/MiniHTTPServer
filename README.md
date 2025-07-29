# Modules / 구성 요소
---

## server (server.h / server.cpp)
**EN:**  
Entry point of the network layer:
- Creates/listens on a TCP socket (`socket()`, `bind()`, `listen()`).
- In `acceptLoop()`, on each `accept()`, constructs `Session(client_fd, router, db)` and calls `handle()`.

**KR:**  
네트워크 진입점 역할:
- TCP 소켓 생성/바인딩/리스닝(`socket()`, `bind()`, `listen()`).
- `acceptLoop()`에서 들어오는 연결마다 `Session(client_fd, router, db)`를 생성해 `handle()` 호출.

---

## router (router.h / router.cpp)
**EN:**  
Manages registration and dispatch of HTTP endpoints:
- `add_route(method, path, handler)`: map `"METHOD PATH"` to a handler function.
- `dispatch(req)`: look up the handler by `req.method + " " + req.path`, with optional prefix‐matching for dynamic paths.

**KR:**  
HTTP 경로와 핸들러를 관리하는 라우터:
- `add_route(method, path, handler)`: `"METHOD PATH"` 키로 핸들러 등록.
- `dispatch(req)`: `req.method + " " + req.path`을 찾아 실행, 동적 경로(prefix 매칭)도 지원.

---

## session (session.h / session.cpp)
**EN:**  
Handles one TCP connection’s HTTP request/response cycle:
1. `read_request()`: accumulates TCP data, parses request‐line, headers, and body.
2. `dispatch(req)`: forwards to `Router`.
3. `send_response(res)`: serializes `Response` and ensures full send().
4. Logs each request via provided `DB` reference.

**KR:**  
하나의 TCP 연결에 대한 HTTP 요청/응답 처리를 담당:
1. `read_request()`: TCP로 들어온 데이터를 모아 요청 라인·헤더·바디 파싱.
2. `dispatch(req)`: 라우터로 전달.
3. `send_response(res)`: `Response` 직렬화 후 전송(전송 루프 포함).
4. `DB` 참조를 통해 요청을 SQLite에 로깅.

---

## main (src/main.cpp)
**EN:**  
Bootstraps the server:
1. Instantiate `Server` on port 8080.
2. Register handlers via `add_route()`:
    - Static files under `/static/...` using `util`.
    - Simple `GET /hello`, `POST /api/echo`.
3. Call `run()` to start the accept–dispatch loop.

**KR:**  
서버 실행부:
1. `Server` 객체를 8080 포트로 생성.
2. `add_route()`로 엔드포인트 등록:
    - `GET /hello`, `POST /api/echo`.
    - `/static/*` 경로로 정적 파일 제공(`util` 사용).
3. `run()` 호출로 클라이언트 연결 수락 및 요청 처리 시작.

---

## util (util.h / util.cpp)
**EN:**  
Provides helper functions for serving static files:
- `read_file(path, ok)`: reads a file into a string, sets `ok` to true on success.
- `mime_type(ext)`: returns the correct `Content-Type` header value based on a file extension.

**KR:**  
정적 파일 제공을 위한 유틸 함수 모음:
- `read_file(path, ok)`: 파일을 읽어 `std::string`으로 반환하고, 성공 시 `ok=true`.
- `mime_type(ext)`: 확장자에 맞는 `Content-Type` 문자열을 반환.