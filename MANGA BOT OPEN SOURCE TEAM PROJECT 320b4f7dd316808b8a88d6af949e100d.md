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