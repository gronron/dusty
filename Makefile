CXX			=	clang++
CXXFLAGS	=	-g2 -O2 -W -Wall
RM			=	rm -rf

LIB			=	-lpthread -lrt -lz -lpng -lfreetype -lGL -lGLEW -lsfml-window -lsfml-system -lfmodex64
INCLUDE		=	-iquote./libsrc \
				-iquote./engine \
				-iquote./graphic \
				-iquote./network \
				-iquote./physic \
				-iquote./ui \
				-iquote./game/actors \
				-iquote./game/animations
VPATH		=	audio:engine:graphic:network:physic:ui:libsrc/df:libsrc/endian:libsrc/math:libsrc/mt:libsrc/stream:libsrc/thread:libsrc/time:game/actors:game/animations

LIBSRCSRC	=	df.cpp \
				packet.cpp \
				mt19937.cpp \
				selector.cpp \
				tcpstream.cpp \
				udpstream.cpp \
				mutex.cpp \
				thread.cpp \
				time.cpp
ENGINESRC	=	actor.cpp \
				actormanager.cpp \
				configmanager.cpp \
				factory.cpp
GRAPHICSRC	=	animation.cpp \
				graphicengine.cpp \
				image.cpp
NETWORKSRC	=	client.cpp \
				messagequeue.cpp \
				networkengine.cpp \
				networkproto.cpp \
				replication.cpp \
				server.cpp
PHYSICSRC	=	body.cpp \
				physicengine.cpp
UISRC		=	console.cpp \
				controller.cpp \
				eventmanager.cpp

ACTORSSRC	=	acontroller.cpp \
				bonus.cpp \
				exit.cpp \
				feeder.cpp \
				labyrinth.cpp \
				level.cpp \
				player.cpp \
				projectile.cpp
ANIMATIONSRC=	hud.cpp \
				leveldisplayer.cpp \
				particlesystem.cpp

DUSTYSRC	=	$(LIBSRCSRC) $(ENGINESRC) $(GRAPHICSRC) $(NETWORKSRC) $(PHYSICSRC) $(UISRC) $(ACTORSSRC) $(ANIMATIONSRC) \
				main.cpp

DUSTYNAME	=	./lib/dusty_unix
DUSTYOBJ	=	$(DUSTYSRC:.cpp=.o)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(DUSTYNAME): $(DUSTYOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB)

all: $(DUSTYNAME)

clean:
	$(RM) $(DUSTYOBJ)

fclean: clean
	$(RM) $(DUSTYNAME)

re:	fclean all
