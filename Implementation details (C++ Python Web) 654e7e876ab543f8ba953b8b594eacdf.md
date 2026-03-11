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