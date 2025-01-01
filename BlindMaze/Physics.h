#pragma once
#include "Vec2.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"

#define PI 3.14159265
class Physics {
public:
    // Checking intersetion between lines
    void intersect(Vec2 a, sf::Vertex& bo, Vec2 c, Vec2 d)
    {
        Vec2 b(bo.position.x, bo.position.y);
        Vec2 r = b - a, s = d - c;
        float rXs = cross(r, s);
        Vec2 cma = c - a;
        float amcXs = cross(cma, s);
        float amcXr = cross(cma, r);

        float t = amcXs / rXs;
        float u = amcXr / rXs;

        //std::cout << c.x << "  " << c.y << "\n";
        //std::cout << d.x << "  " << d.y << "\n";

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
        {
            b = a + (r * t);
            bo.position = sf::Vector2f(b.x, b.y);
        }
    }

    void lightEffect(std::vector<sf::Vertex>& vectors, const std::shared_ptr<Entity> player, const float distance)
    {
        auto& cp = player->getComponent<CTransform>().pos;
        sf::Vector2f ls(cp.x, cp.y);
        for (auto& v : vectors)
        {
            v.color = calculateGradient(v.position, ls, distance); 
        }
    }

    // 2d cross product
    float cross(const Vec2& rhs1, const Vec2& rhs2)
    {
        return (rhs1.x * rhs2.y) - (rhs1.y * rhs2.x);
    }

    std::vector<sf::Vertex> getRectanglePoints( std::vector<sf::Vertex>& vectors,std::vector<std::shared_ptr<Entity>>& boxes, const std::shared_ptr<Entity> player, sf::RenderWindow& window, const sf::Vector2f& m_pos, const float scope, const float length)
    {
        auto winPos = window.getSize();
        auto& cp = player->getComponent<CTransform>().pos;

        /*validateVector(vectors,
            sf::Vertex(sf::Vector2f(0, 0), sf::Color::Black),
            m_pos, cp, scope / 2);

        validateVector(vectors,
            sf::Vertex(sf::Vector2f(0, winPos.y), sf::Color::Black),
            m_pos, cp, scope / 2);

        validateVector(vectors,
            sf::Vertex(sf::Vector2f(winPos.x, winPos.y), sf::Color::Black),
            m_pos, cp, scope / 2);

        validateVector(vectors,
            sf::Vertex(sf::Vector2f(winPos.x, 0), sf::Color::Black),
            m_pos, cp, scope / 2);*/


        for (auto& b : boxes)
        {
            auto& temp = b->getComponent<CVertex>().vertex;
            for (int i = 0; i < 4; i++)
            {
                Vec2 normal(temp[i].position.x, temp[i].position.y);
                normal.normalize();
                auto pos = sf::Vector2f(normal.x + cp.x, normal.y + cp.x) ;

                /*validateVector(vectors,
                    sf::Vertex(pos, sf::Color(i*5, i*5, i*5)), 
                    m_pos, cp, scope / 2);*/
                float angle = vectorToDegree(cp, temp[0]);

                //std::cout << cos(toRadian(angle+30)) + cp.x << " " << sin(toRadian(angle+30)) + cp.y << "\n";
                float off = 0.001f;
                validateVector(vectors,
                    sf::Vertex(sf::Vector2f(cos(toRadian(angle) + off) * length + cp.x, sin(toRadian(angle) + off) * length + cp.y), sf::Color::Black),
                    m_pos, cp, scope);
                validateVector(vectors,
                    sf::Vertex(sf::Vector2f(cos(toRadian(angle) - off) * length + cp.x, sin(toRadian(angle) - off) * length + cp.y), sf::Color::Black),
                    m_pos, cp, scope);
            }

        }
        return vectors;
    }

    void validateVector(std::vector<sf::Vertex>& vectors, const sf::Vertex& vertex, const sf::Vector2f& m_pos, const Vec2& cp, const float scope)
    {

        float mouse_angle = vectorToDegree(cp, m_pos);
        float vertex_angle = vectorToDegree(cp, vertex.position);
        if (scope > 180)
        {
            vectors.push_back(vertex);
        }
        else if (mouse_angle <= scope/2)
        {
            float remainingmin = std::abs(mouse_angle - scope / 2);
            if (vertex_angle >= 0 && vertex_angle <= mouse_angle + scope / 2 ||
                vertex_angle >= 360 - remainingmin)
            {
                vectors.push_back(vertex); 
            }
        }
        else if (mouse_angle + scope / 2 >= 360)
        {
            float remainingplus = std::abs((mouse_angle + scope / 2) - 360);
            if (vertex_angle >= mouse_angle - scope / 2 && vertex_angle <= mouse_angle + scope / 2 ||
                vertex_angle >= 0 && vertex_angle <= remainingplus)
            {
                vectors.push_back(vertex);
            }
        }
        else
        {
            if (vertex_angle >= mouse_angle - scope / 2 && vertex_angle <= mouse_angle + scope / 2)
            {
                vectors.push_back(vertex);
            }
        }
    }

    float toRadian(double angle)
    {
        return angle * PI / 180;
    }


    void sortVector(std::vector<sf::Vertex>& vectors, Vec2 cp, const sf::Vector2f& m_pos, float scope)
    {
        auto angle = mesureAngle(vectors, cp, m_pos, scope);
        std::vector<sf::Vertex> vecTemp = vectors;

        for (int i = 0; i < vecTemp.size(); i++)
        {
            vectors[i] = vecTemp[angle[i].first];
        }

    }


    std::vector<std::pair<int, float>> mesureAngle(std::vector<sf::Vertex>& vectors, Vec2 cp, const sf::Vector2f& m_pos, float scope)
    {
        float mPos = vectorToDegree(cp, m_pos);
        std::vector<std::pair<int, float>> result;
        if (scope > 180)
        {
            for (int i = 0; i < vectors.size(); i++)
            {
                result.push_back(std::make_pair(i, vectorToDegree(cp, vectors[i]))); 
            }
        }
        else if (mPos <= scope / 2 || mPos >= 360 - scope / 2)
        {
            //std::cout << "true" << std::endl;
            for (int i = 0; i < vectors.size(); i++)
            {
                result.push_back(std::make_pair(i, vectorToDegreeCustom(cp, vectors[i], mPos, scope)));
            }
        }
        else
        {
            for (int i = 0; i < vectors.size(); i++)
            {
                result.push_back(std::make_pair(i, vectorToDegree(cp, vectors[i])));
            }
        }

        sortMap(result);

        return result;
    }

    float vectorToDegree(const Vec2&  startPoint, const sf::Vertex& vec)
    {
        float angleL = atan2(startPoint.y - vec.position.y, startPoint.x - vec.position.x);
        angleL = (float)(angleL * 180 / PI);
        if (angleL <= 0)
        {
            angleL += 360;
        }

        return angleL;
    }

    float vectorToDegreeCustom(const Vec2& startPoint, const sf::Vertex& vec, float m, float scope)
    {
        float left = m - (scope / 2);
        if (m >= 0 && m <= scope / 2)
        {
            left += 360;
        }
        float angleL = atan2(startPoint.y - vec.position.y, startPoint.x - vec.position.x);
        if (angleL >= left && angleL <= 360)
        {
            return angleL - left;
        }
        else if (angleL >= 0 && angleL <= m + (scope / 2))
        {
            return angleL - left + 361;
        }
        return 0;
    }
    float vectorToDegree(const Vec2& startPoint, const sf::Vector2f& vec)
    {
        float angleL = atan2(startPoint.y - vec.y, startPoint.x - vec.x);
        angleL = (float)(angleL * 180 / PI);
        if (angleL <= 0)
        {
            angleL += 360;
        }

        return angleL;
    }

    std::vector<std::pair<int, float>> sortMap(std::vector<std::pair<int, float>>& myMap)
    {
        // Copy the elements to a vector
        std::vector<std::pair<int, float>> vec(myMap.begin(), myMap.end());

        // Sort the vector based on the float value (ascending)
        std::sort(myMap.begin(), myMap.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
            return a.second < b.second;
            });
        return myMap;
    }

    sf::Color calculateGradient(const sf::Vector2f& point, const sf::Vector2f& lightSource, float maxDistance) {
        // Calculate the distance between the point and the light source
        float distance = std::sqrt(std::pow(point.x - lightSource.x, 2) + std::pow(point.y - lightSource.y, 2));

        // Clamp the distance to the maximum distance
        distance = std::min(distance, maxDistance);

        // Calculate the intensity (0 = dark, 1 = bright)
        float intensity = 1.0f - (distance / maxDistance);

        // Return the color with adjusted intensity
        return sf::Color(
            static_cast<sf::Uint8>(255 * intensity),    // Red
            static_cast<sf::Uint8>(255 * intensity),    // Green
            static_cast<sf::Uint8>(255 * intensity),    // Blue
            static_cast<sf::Uint8>(255 * intensity)     // Alpha
        );
    }

    std::vector<sf::Vertex> getAllDirection( std::vector<sf::Vertex>& vectors,const int& vertices, const std::shared_ptr<Entity>& player, const sf::Vector2f& m_pos, const float scope, const float length)
    {
        int step = 360 / vertices;
        auto& cp = player->getComponent<CTransform>().pos;
        for (int i = 0; i < vertices; i++)
        {
            float velx = (float)cos(i * step * PI / 180);
            float vely = (float)sin(i * step * PI / 180);
            auto vertex = sf::Vertex(sf::Vector2f((velx * length) + cp.x, (vely * length) + cp.y), sf::Color::Black);
            //std::cout << vertex.position.x << " " << vertex.position.y << " va pos\n";
            validateVector(vectors, vertex, m_pos, cp, scope);
        }

        return vectors;
    }

    void IntersectRay(std::vector<sf::Vertex>& angle, const Vec2& pPos, const std::vector<std::shared_ptr<Entity>> entities)
    {
        for (auto& e : angle)
        {
            //std::cout << e.position.x << " " << e.position.y << std::endl;
            for (const auto& wall : entities)
            {
                const auto& f = wall->getComponent<CVertex>().vertex;
                intersect(Vec2(pPos.x, pPos.y), e,
                    Vec2(f[0].position.x, f[0].position.y),
                    Vec2(f[1].position.x, f[1].position.y));

                intersect(Vec2(pPos.x, pPos.y), e,
                    Vec2(f[1].position.x, f[1].position.y),
                    Vec2(f[2].position.x, f[2].position.y));

                intersect(Vec2(pPos.x, pPos.y), e,
                    Vec2(f[2].position.x, f[2].position.y),
                    Vec2(f[3].position.x, f[3].position.y));

                intersect(Vec2(pPos.x, pPos.y), e,
                    Vec2(f[3].position.x, f[3].position.y),
                    Vec2(f[0].position.x, f[0].position.y));

            }
        }
    }

    void addLight(std::vector<sf::Vertex*>& light, const std::vector<sf::Vertex>& angle, const Vec2& pPos, int off)
    {
        light.clear();
        for (int i = 0; i < angle.size() - off; i++)
        {
            if (i + 1 >= angle.size())
            {
                light.push_back(
                    new sf::Vertex[3]{
                       sf::Vertex(sf::Vector2f(pPos.x, pPos.y), sf::Color::White),
                       angle[i],
                       angle[0],
                    }
                    );
                break;
            }
            light.push_back(
                new sf::Vertex[3]{
                   sf::Vertex(sf::Vector2f(pPos.x, pPos.y), sf::Color::White),
                   angle[i],
                   angle[i + 1],
                }
                );
        }
    }

    void addRay(std::vector<sf::Vertex*>& ray, const std::vector<sf::Vertex>& angle, const Vec2& pPos)
    {
        for (auto e : angle)
        {
            ray.push_back(
                new sf::Vertex[2]{
                sf::Vertex(sf::Vector2f(pPos.x, pPos.y), sf::Color::Red),
                sf::Vertex(sf::Vector2f(e.position.x, e.position.y), sf::Color::Red),
                }
                );
        }
    }
};

