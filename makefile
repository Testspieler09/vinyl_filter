output: audio_to_vinyl.o filehandler.o filters.o
	g++ audio_to_vinyl.o filehandler.o filters.o -o output

audio_to_vinyl.o: audio_to_vinyl.cpp
	g++ -c audio_to_vinyl.cpp

filehandler.o: filehandler.cpp
	g++ -c filehandler.cpp

filters.o: filters.cpp
	g++ -c filters.cpp

clean:
	rm *.o output
