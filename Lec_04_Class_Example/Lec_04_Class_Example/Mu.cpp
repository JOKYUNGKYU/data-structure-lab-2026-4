#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Music Class
class Music {
private:
	string title;
	string artist;
	string album;
	int year;

public:
	Music(string title, string artist, string album, int year) {
		this->title = title;
		this->artist = artist;
		this->album = album;
		this->year = year;
	}
	string getTitle() { return title; }
	string getArtist() { return artist; }
	string getAlbum() { return album; }
	string getYear() { return year; }
};

// Music Streaming Streaming Service Class

public:
	MusicStreamingService(string name) {
		this->serviceName = name;
	}
	// Add a music to the service
	void addmusic(string title, string artist, string album, int year) {
		Music newMusic(title, artist, album, year);
		musicList.push_back(newMusic);
		cout << title << " by " << artist << " added to " << serviceName << endl;
	}