#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <math.h>
#include <iostream>

const float sep_mul = 10000;
const float coh_mul = 10;
const float ali_mul = 1;
const float fear_mul = 100000;
const float time_step = 0.001;


float fast_sqrt(float input)
{
    return sqrt(input);
}

class Boid
{
private:
    sf::CircleShape shape;
    sf::Vector2f vel;
public:
    Boid(sf::Vector2f pos)
    {
        shape = sf::CircleShape(5, 3);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(pos);
    }

    void move(const float dt)
    {
        shape.move(vel.x*dt, vel.y*dt);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    sf::Vector2f get_pos() const
    {
        return shape.getPosition();
    }

    sf::Vector2f get_vel() const
    {
        return vel;
    }

    float distance(const Boid &another) const
    {
        float dif_x = another.get_pos().x - get_pos().x;
        float dif_y = another.get_pos().y - get_pos().y;
        return fast_sqrt((dif_x*dif_x) + (dif_y*dif_y));
    }

    void draw(sf::RenderWindow &window)
    {const float sep_mul = 0.4;
        window.draw(shape);
    }

    void apply_acc(sf::Vector2f acc, const float dt)
    {
        vel.x += acc.x*dt;
        vel.y += acc.y*dt;
    }


};

float lenght(sf::Vector2f a)
{
    return fast_sqrt((a.x*a.x) + (a.y*a.y));
}

sf::Vector2f normalize(sf::Vector2f a)
{
    float len = lenght(a);
    return sf::Vector2f(a.x/len, a.y/len);
}


void separation(Boid &chosenboid, const std::vector<Boid> &boids, float sight)
{
    for(int i=0;i<boids.size();i++)
    {
        if(&chosenboid != &boids[i])
        {
            float dist = chosenboid.distance(boids[i]);
            if(dist < sight)
            {
                sf::Vector2f helper = chosenboid.get_pos() - boids[i].get_pos();
                sf::Vector2f acc = normalize(helper);
                float len = lenght(helper);
                acc.x *= (sep_mul/len);
                acc.y *= (sep_mul/len);
                chosenboid.apply_acc(acc, time_step);
            }

        }
    }
}

void cohesion(Boid &chosenboid, const std::vector<Boid> &boids, float sight)
{
    sf::Vector2f mean; //= chosenboid.get_pos();
    float n = 0;
    for(int i=0;i<boids.size();i++)
    {
        if(&chosenboid != &boids[i])
        {
            float dist = chosenboid.distance(boids[i]);
            if(dist < sight)
            {
                mean.x = (n*mean.x + boids[i].get_pos().x)/(n+1.0);
                mean.y = (n*mean.y + boids[i].get_pos().y)/(n+1.0);
                n++;
            }

        }
    }

    if(n == 0)
    {
        return;
    }

    sf::Vector2f helper = mean - chosenboid.get_pos();
    sf::Vector2f acc = normalize(helper);
    float len = lenght(helper);
    acc.x *= (coh_mul*len);
    acc.y *= (coh_mul*len);
    chosenboid.apply_acc(acc, time_step);
}

void alignment(Boid &chosenboid, const std::vector<Boid> &boids, float sight)
{
    sf::Vector2f mean = chosenboid.get_vel();
    float n = 0;
    for(int i=0;i<boids.size();i++)
    {
        if(&chosenboid != &boids[i])
        {
            float dist = chosenboid.distance(boids[i]);
            if(dist < sight)
            {
                mean.x = (n*mean.x + boids[i].get_vel().x)/(n+1.0);
                mean.y = (n*mean.y + boids[i].get_vel().y)/(n+1.0);
                n++;
            }

        }
    }

    if(n == 0)
    {
        return;
    }
    mean.x *= ali_mul;
    mean.y *= ali_mul;
    chosenboid.apply_acc(mean - chosenboid.get_vel(), time_step);
}

void fear(Boid &chosenboid, sf::Vector2f what, float sight)
{
    float dist = lenght(chosenboid.get_pos() - what);
    if(dist < sight)
    {
        sf::Vector2f helper = chosenboid.get_pos() - what;
        sf::Vector2f acc = normalize(helper);
        float len = lenght(helper);
        acc.x *= (fear_mul/len);
        acc.y *= (fear_mul/len);
        chosenboid.apply_acc(acc, time_step);
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Boids!");

    std::vector<Boid> boids;

    for(int i=0;i<20;i++)
    {
        boids.push_back(Boid(sf::Vector2f(50+i*50,200 + (i%2)*50)));
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        for(int i=0;i<boids.size();i++)
        {
            separation(boids[i], boids, 100);
            cohesion(boids[i], boids, 100);
            alignment(boids[i], boids, 100);
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            fear(boids[i], sf::Vector2f(mouse.x, mouse.y), 100);
        }

        for(int i=0;i<boids.size();i++)
        {
            boids[i].move(time_step);
            boids[i].draw(window);
        }
        window.display();
    }

    return 0;
}
