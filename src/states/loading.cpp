#include "states/loading.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


LoadingState::LoadingState(StateStack& stack, Context context)
    : State(stack, context)
{
    sf::RenderWindow& window = *getContext().window;
    sf::Vector2f viewSize = window.getView().getSize();

    mLoadingText.setString("Loading Resources");
    mLoadingText.setPosition(viewSize.x / 2.f, viewSize.y / 2.f + 50.f);
    auto bounds = mLoadingText.getLocalBounds();
    mLoadingText.setOrigin((bounds.left + bounds.width) / 2.f, (bounds.top + bounds.height) / 2.f);

    mProgressBarBackground.setFillColor(sf::Color::White);
    mProgressBarBackground.setSize(sf::Vector2f(viewSize.x - 20, 10));
    mProgressBarBackground.setPosition(10, mLoadingText.getPosition().y + 40);

    mProgressBar.setFillColor(sf::Color(100,100,100));
    mProgressBar.setSize(sf::Vector2f(200, 10));
    mProgressBar.setPosition(10, mLoadingText.getPosition().y + 40);

    setCompletion(0.f);

    //mLoadingTask.execute();
}

void LoadingState::draw()
{
    sf::RenderWindow& window = *getContext().window;

    window.setView(window.getDefaultView());

    window.draw(mLoadingText);
    window.draw(mProgressBarBackground);
    window.draw(mProgressBar);
}

bool LoadingState::update(sf::Time)
{
    return true;
}

bool LoadingState::handleEvent(const sf::Event&)
{
    return true;
}

void LoadingState::setCompletion(float percent)
{
    if(percent > 1.f) { // clamp
        percent = 1.f;
    }

    mProgressBar.setSize(sf::Vector2f(mProgressBarBackground.getSize().x * percent, mProgressBar.getSize().y));
}
