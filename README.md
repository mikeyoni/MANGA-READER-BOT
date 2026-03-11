# MANGA BOT OPEN SOURCE TEAM PROJECT

CONCEPT:

This will be an easy-to-host Discord bot that people can use to read manga in Discord. We are not going to host it like other big Discord bots. We may host it using free hosting so people can try and test it initially.

It will be like people are going to host it on their system and use it with their friend, or use it as their product. It is going to be open source so people can use it to sell it with their product. But we are going to add some credit for the contributor in the main C++ engine in the binary code, so people can’t remove the contributor credit even by changing the Python-written bot code.

Multilingual:

C++, Python, [HTML, CSS, JS]

C++:

C++ will be used for the main web scraping: getting info like the manga name, manga title, and manga thumbnail from the web. It will also handle the search system in C++, fulfilling the requests from Discord via a JSON file. It will communicate, give instructions, and receive `requests.json` instructions from the Discord bot. It will also be used to write the contributor info in the main engine.

PYTHON:

Python will be used to make the actual Discord bot with [discord.py](http://discord.py). It will make JSON files to give instructions to the C++ engine. It will receive the manga pages in image form from the C++ engine, and the image location and name will also be in a JSON file. It will communicate the most with the user, so it will also filter users. If any spam requests or command spamming happens, it will ban the user permanently. It will also send the bot info to a web server: info like bot status and uptime, requests, error logs.

HTML, CSS, JavaScript:

We will also make an admin panel like a web server with password protection. These are only for the contributors. We will be able to see bot error logs, how many bots are active in real time, how many are working, and how many requests are made. We will also be able to see which user was banned. Also, we can make announcements to all bots from the admin panel web page. This will give contributors admin power.

### Technical details (code + algorithms)

[Implementation details (C++ / Python / Web)](MANGA%20BOT%20OPEN%20SOURCE%20TEAM%20PROJECT/Implementation%20details%20(C++%20Python%20Web)%20654e7e876ab543f8ba953b8b594eacdf.md)


# Implementation details (C++ / Python / Web)

<aside>
🧠

This page is for the **real implementation plan**: code structure, logic, and algorithms.

</aside>

### 1) High-level architecture

- **Discord bot (Python / [discord.py](http://discord.py))**: handles commands, permissions, UI, rate limiting, and chooses what to request.
- **Engine (C++)**: scraping, parsing, search indexing, image processing, and returning results.
- **Shared contract (JSON)**: Python sends a request JSON to C++. C++ returns a response JSON.
- **Web admin panel (HTML/CSS/JS + server)**: collects logs, shows stats, and sends announcements.

### 2) Shared JSON contract (Python ⇄ C++)

**Goals**

- Stable across versions
- Easy to debug
- Backward compatible when possible

**Request JSON: `request.json`**

```json
{
	"request_id": "uuid-or-increment",
	"action": "search|info|chapter_list|pages|health",
	"source": "mangadex|site_x|site_y",
	"query": "one piece",
	"manga_id": "optional",
	"chapter_id": "optional",
	"language": "en",
	"page": 1,
	"limit": 10,
	"cache": true
}
```

**Response JSON: `response.json`**

```json
{
	"request_id": "same-as-request",
	"ok": true,
	"error": null,
	"data": {
		"items": []
	}
}
```

**Error model**

- `ok=false`
- `error` object: `{ "code": "SCRAPE_FAILED", "message": "...", "retryable": true }`

### 3) C++ (Engine) — scraping + algorithms

#### A) Folder structure suggestion

```
engine/
	src/
		main.cpp
		api/
			request_parser.cpp
			response_writer.cpp
		scrapers/
			site_x_scraper.cpp
			mangadex_scraper.cpp
		core/
			search_index.cpp
			cache.cpp
			rate_limit.cpp
			credits.cpp
	include/
	third_party/
```

#### B) Main algorithm flow

1. Read `request.json`
2. Validate fields (action, required IDs)
3. Route to handler (search/info/chapter_list/pages)
4. Scrape or fetch cached result
5. Write `response.json`

#### C) Scraping pipeline (per site)

- Fetch HTML
- Parse DOM
- Extract:
    - Title
    - Thumbnail URL
    - Manga ID
    - Chapter list
    - Page image URLs
- Normalize outputs into one common internal model

#### D) Search algorithm options

**Option 1: Simple (good start)**

- Keep an in-memory list of titles per site
- On `search`, do:
    - lowercase
    - remove punctuation
    - substring match + basic scoring

**Option 2: Better scoring (still simple)**

- Tokenize query and titles
- Score = token overlap + prefix bonus + length penalty
- Return top N

#### E) Caching strategy

- Cache by `(source, action, manga_id, chapter_id, language)`
- Disk cache for:
    - chapter list
    - page URLs
- Time-based expiry (example: 6 hours)

#### F) Pages output

- C++ downloads images (or streams them) and outputs:
    - file paths
    - width/height if available
    - page count

Example `data.pages` payload:

```json
{
	"pages": [
		{ "index": 1, "path": "./cache/onepiece/1071/001.jpg" },
		{ "index": 2, "path": "./cache/onepiece/1071/002.jpg" }
	]
}
```

#### G) Contributor credits (harder to remove)

**Practical approach**

- Store credits as:
    - a compiled-in string table
    - displayed via a command like `/credits`
    - also included in engine `--version` output

**Harder-to-remove ideas (still not perfect)**

- Spread credits across multiple translation units
- Validate checksum of credit strings at runtime
- Fail safe if tampered (but be careful: this can annoy legitimate forks)

### 4) Python ([discord.py](http://discord.py)) — bot logic + safety

#### A) Folder structure suggestion

```
bot/
	bot.py
	cog_reader.py
	cog_admin.py
	engine_client.py
	config.py
	storage.py
	anti_spam.py
```

#### B) Bot flow per user command

1. Parse command (example: `/manga search <query>`)
2. Check permissions + server settings
3. Apply rate limits (per-user + per-guild)
4. Build `request.json`
5. Run the C++ engine (subprocess) or call it as a local service
6. Parse `response.json`
7. Render response:
    - embeds
    - buttons for next/prev page
    - select menus for chapters

#### C) Anti-spam algorithm (simple and effective)

- Keep a sliding window per user:
    - `N` commands in `T` seconds
- If exceeded:
    - warn
    - temporary cooldown
- Repeat offenders:
    - add to ban list

Data model:

- `banned_users.json`
- `cooldowns.json`

#### D) Instruction building (what to send to C++)

- Always include `request_id`
- Always include `action`
- Only include IDs when needed
- Keep a `schema_version` field once you start evolving the contract

### 5) Web (Admin panel) — logs, stats, announcements

#### A) Minimal components

- **Collector API** (server): receives bot heartbeats + logs
- **Dashboard** (web UI): shows bots online, errors, request counts
- **Auth**: password login for contributors

#### B) Data to send from bots

- `bot_id`
- `guild_count`
- `uptime_seconds`
- `requests_last_1h`
- `errors_last_1h`
- `engine_version`

#### C) Announcement flow

1. Admin creates announcement in panel
2. Server stores it + assigns `announcement_id`
3. Bots poll periodically or receive a push
4. Bot posts announcement to configured channels
