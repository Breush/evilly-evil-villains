#pragma once

#include "states/identifiers.hpp"
#include "resources/identifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

namespace sf
{
    class RenderWindow;
}

class StateStack;
class MusicPlayer;
class SoundPlayer;

class State
{
public:
    typedef std::unique_ptr<State> Ptr;

    struct Context {
        Context(sf::RenderWindow& window, TextureHolder& textures, ShaderHolder& shaders,
                FontHolder& fonts, MusicPlayer& music, SoundPlayer& sounds);

        sf::RenderWindow* window;
        TextureHolder* textures;
        ShaderHolder* shaders;
        FontHolder*	fonts;
        MusicPlayer* music;
        SoundPlayer* sounds;
    };


public:
    State(StateStack& stack, Context context);
    virtual	~State();

    virtual void draw() = 0;
    virtual bool update(sf::Time dt) = 0;
    virtual bool handleEvent(const sf::Event& event) = 0;

    virtual void onShow() {}
    virtual void onHide() {}

protected:
    void stackPopPush(States::ID stateID);
    void stackPush(States::ID stateID);
    void stackPop();
    void stackClear();

    Context	getContext() const;

private:
    StateStack*			mStack;
    Context				mContext;
};
