#pragma once

#include "states/state.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>


class LoadingState : public State
{
public:
    LoadingState(StateStack& stack);

    virtual void			draw();
    virtual bool			update(sf::Time dt);
    virtual bool			handleEvent(const sf::Event& event);

    void					setCompletion(float percent);

private:
    sf::Text				mLoadingText;
    sf::RectangleShape		mProgressBarBackground;
    sf::RectangleShape		mProgressBar;

    //ParallelTask			mLoadingTask;
};
