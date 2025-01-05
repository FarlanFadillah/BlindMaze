#pragma once
#include "Vec2.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"

constexpr auto PI = 3.14159265;
class Physics {
public:
    // Checking intersetion between lines
    void intersect(const sf::Vector2f& a, sf::Vertex& bo, const sf::Vector2f& c, const sf::Vector2f& d)
    {
        sf::Vector2f b(bo.position.x, bo.position.y);
        sf::Vector2f r(b - a), s(d - c);
        float rXs = cross(r, s);
        sf::Vector2f cma(c - a);
        float amcXs(cross(cma, s));
        float amcXr(cross(cma, r));

        float t(amcXs / rXs);
        float u(amcXr / rXs);

        //std::cout << c.x << "  " << c.y << "\n";
        //std::cout << d.x << "  " << d.y << "\n";

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
        {
            b = a + (r * t);
            bo.position = sf::Vector2f(b.x, b.y);
        }
    }

    bool isIntersect(const sf::Vector2f& a, sf::Vertex& bo, const sf::Vector2f& c, const sf::Vector2f& d)
    {
        sf::Vector2f b(bo.position.x, bo.position.y);
        sf::Vector2f r(b - a), s(d - c);
        float rXs = cross(r, s);
        sf::Vector2f cma(c - a);
        float amcXs(cross(cma, s));
        float amcXr(cross(cma, r));

        float t(amcXs / rXs);
        float u(amcXr / rXs);

        //std::cout << c.x << "  " << c.y << "\n";
        //std::cout << d.x << "  " << d.y << "\n";

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
        {
            b = a + (r * t);
            bo.position = sf::Vector2f(b.x, b.y);
            return true;
        }
        return false;
    }

    void lightEffect(std::vector<sf::Vertex>& vectors, const Vec2& cp, const float distance)
    {
        sf::Vector2f ls(cp.x, cp.y);
        for (auto& v : vectors)
        {
            v.color = calculateGradient(v.position, ls, distance); 
        }
    }

    // 2d cross product
    float cross(const sf::Vector2f& rhs1, const sf::Vector2f& rhs2)
    {
        return (rhs1.x * rhs2.y) - (rhs1.y * rhs2.x);
    }

    std::vector<sf::Vertex> getAllDirection(std::vector<sf::Vertex>& vectors, const int vertices, const Vec2& cp, const sf::Vector2f& m_pos, const float scope, const float length)
    {
        int step = 360 / vertices;
        for (int i = 0; i < vertices; i++)
        {
            float velx = (float)cos(i * step * PI / 180);
            float vely = (float)sin(i * step * PI / 180);
            sf::Vertex vertex(sf::Vector2f((velx * length) + cp.x, (vely * length) + cp.y), sf::Color::Black); 
            //std::cout << vertex.position.x << " " << vertex.position.y << " va pos\n";
            validateVector(vectors, vertex, m_pos, cp, scope);
        }

        return vectors;
    }

    void getRectanglePoints( std::vector<sf::Vertex>& vectors,std::vector<std::shared_ptr<Entity>>& boxes, const Vec2& cp, const sf::Vector2f& m_pos, const float scope, const float length)
    {
       
        for (auto& b : boxes)
        {
            for (const auto& temp : b->getComponent<CVertex>().vertex)
            {
                if (outOfRange(length, temp.position, cp)) continue;
                Vec2 normal(temp.position.x, temp.position.y);
                normal.normalize();
                sf::Vector2f pos(normal.x + cp.x, normal.y + cp.x);

                /*validateVector(vectors,
                    sf::Vertex(pos, sf::Color(i*5, i*5, i*5)), 
                    m_pos, cp, scope / 2);*/
                float angle = vectorToDegree(sf::Vector2f(cp.x, cp.y), temp.position);

                //std::cout << cos(toRadian(angle+30)) + cp.x << " " << sin(toRadian(angle+30)) + cp.y << "\n";
                float off(0.001f);
                validateVector(vectors,
                    sf::Vertex(sf::Vector2f(cos(toRadian(angle) + off) * length + cp.x, sin(toRadian(angle) + off) * length + cp.y), sf::Color::Black),
                    m_pos, cp, scope);
                validateVector(vectors,
                    sf::Vertex(sf::Vector2f(cos(toRadian(angle) - off) * length + cp.x, sin(toRadian(angle) - off) * length + cp.y), sf::Color::Black),
                    m_pos, cp, scope);
            }

        }
    }

    void getStaticRectanglePoints(std::vector<CVertex>& boxes, std::vector<std::shared_ptr<Entity>>& entities, const Vec2& cp, const sf::Vector2f& m_pos, const float scope, const float length)
    {
        for (auto& b : entities)
        {
            auto& bt = b->getComponent<CTransform>();
            if (outOfRange(length, sf::Vector2f(bt.pos.x, bt.pos.y), cp)) continue;
            boxes.push_back(b->getComponent<CVertex>());
        }
    }

    void setStaticRectanglePoints(std::vector<sf::Vertex>& vectors, std::vector<CVertex>& boxes, const Vec2& cp, const sf::Vector2f& m_pos, const float scope, const float length)
    {

        for (auto& b : boxes)
        {
            for (const auto& temp : b.vertex)
            {
                if (outOfRange(length, temp.position, cp)) continue;
                Vec2 normal(temp.position.x, temp.position.y);
                normal.normalize();
                sf::Vector2f pos(normal.x + cp.x, normal.y + cp.x);

                /*validateVector(vectors,
                    sf::Vertex(pos, sf::Color(i*5, i*5, i*5)),
                    m_pos, cp, scope / 2);*/
                float angle = vectorToDegree(sf::Vector2f(cp.x, cp.y), temp.position);

                //std::cout << cos(toRadian(angle+30)) + cp.x << " " << sin(toRadian(angle+30)) + cp.y << "\n";
                float off(0.001f);
                validateVector(vectors,
                    sf::Vertex(sf::Vector2f(cos(toRadian(angle) + off) * length + cp.x, sin(toRadian(angle) + off) * length + cp.y), sf::Color::Black),
                    m_pos, cp, scope);
                validateVector(vectors,
                    sf::Vertex(sf::Vector2f(cos(toRadian(angle) - off) * length + cp.x, sin(toRadian(angle) - off) * length + cp.y), sf::Color::Black),
                    m_pos, cp, scope);
            }
        }
    }

    bool outOfRange(float length, const sf::Vector2f& vec, const Vec2& cp)
    {
        Vec2 temp(vec.x - cp.x, vec.y - cp.y);

        return temp.length() > length;
    }

    void validateVector(std::vector<sf::Vertex>& vectors, const sf::Vertex& vertex, const sf::Vector2f& m_pos, const Vec2& cp, const float scope)
    {

        float mouse_angle = vectorToDegree(sf::Vector2f(cp.x, cp.y), m_pos);
        float vertex_angle = vectorToDegree(sf::Vector2f(cp.x, cp.y), vertex.position);
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


    void sortVector(std::vector<sf::Vertex>& vectors, const Vec2& cp, const sf::Vector2f& m_pos, const float scope)
    {
        auto angle = mesureAngle(vectors, cp, m_pos, scope);
        std::vector<sf::Vertex> vecTemp = vectors;

        for (int i = 0; i < vecTemp.size(); i++)
        {
            vectors[i] = vecTemp[angle[i].first];
        }
    }


    std::vector<std::pair<int, float>> mesureAngle(std::vector<sf::Vertex>& vectors, const Vec2& cp, const sf::Vector2f& m_pos, const float scope)
    {
        float mPos = vectorToDegree(sf::Vector2f(cp.x, cp.y), sf::Vector2f(m_pos.x, m_pos.y));
        std::vector<std::pair<int, float>> result;
        if (scope > 180)
        {
            for (int i = 0; i < vectors.size(); i++)
            {
                result.push_back(std::make_pair(i, vectorToDegree(sf::Vector2f(cp.x, cp.y), vectors[i])));
            }
        }
        else if (mPos <= scope / 2 || mPos >= 360 - scope / 2)
        {
            for (int i = 0; i < vectors.size(); i++)
            {
                float deg = vectorToDegreeCustom(cp, vectors[i]);
                result.push_back(std::make_pair(i, deg));
            }
        }
        else
        {
            for (int i = 0; i < vectors.size(); i++)
            {
                result.push_back(std::make_pair(i, vectorToDegree(sf::Vector2f(cp.x, cp.y), vectors[i])));
            }
        }

        sortMap(result);

        /*for (auto& e : result)
        {
            std::cout << e.first << " " << e.second << std::endl;
        }*/

        return result;
    }

    float vectorToDegree(const sf::Vector2f& startPoint, const sf::Vertex& vec)
    {
        float angleL = atan2(startPoint.y - vec.position.y, startPoint.x - vec.position.x);
        angleL = (float)(angleL * 180 / PI) - 180;
        if (angleL < 0)
        {
            angleL += 360;
        }
        //std::cout << angleL << std::endl;
        return angleL;
    }

    float vectorToDegreeCustom(const Vec2& startPoint, const sf::Vertex& vec)
    {
        float angleL = atan2(startPoint.y - vec.position.y, startPoint.x - vec.position.x);
        angleL = (float)(angleL * 180 / PI);
        if (angleL < 0)
        {
            angleL += 360;
        }
        //std::cout << angleL << std::endl;
        return angleL;
    }

    float vectorToDegree(const sf::Vector2f& startPoint, const sf::Vector2f& vec)
    {
        float angleL = atan2(startPoint.y - vec.y, startPoint.x - vec.x);
        angleL = (float)(angleL * 180 / PI) - 180;
        if (angleL < 0)
        {
            angleL += 360;
        }
        //std::cout << angleL << std::endl;
        return angleL;
    }

    std::vector<std::pair<int, float>>& sortMap(std::vector<std::pair<int, float>>& myMap)
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

    void IntersectRay(std::vector<sf::Vertex>& angle, const sf::Vector2f& pPos, const std::vector<std::shared_ptr<Entity>>& entities)
    {
        for (auto& e : angle)
        {
            //std::cout << e.position.x << " " << e.position.y << std::endl;
            for (const auto& wall : entities)
            {
                const auto& f = wall->getComponent<CVertex>().vertex;
                intersect(pPos, e, f.at(0).position,f.at(1).position);
                intersect(pPos, e, f.at(1).position, f.at(2).position);
                intersect(pPos, e, f.at(2).position, f.at(3).position);
                intersect(pPos, e, f.at(3).position, f.at(0).position);

            }
        }
    }

    void IntersectStaticRay(std::vector<sf::Vertex>& angle, const sf::Vector2f& pPos, std::vector<CVertex>& boxes)
    {
        for (auto& e : angle)
        {
            //std::cout << e.position.x << " " << e.position.y << std::endl;
            for (const auto& wall : boxes)
            {
                const auto& f = wall.vertex;
                intersect(pPos, e, f.at(0).position, f.at(1).position);
                intersect(pPos, e, f.at(1).position, f.at(2).position);
                intersect(pPos, e, f.at(2).position, f.at(3).position);
                intersect(pPos, e, f.at(3).position, f.at(0).position);

            }
        }
    }

    void addLight(std::vector<sf::Vertex*>& light, const std::vector<sf::Vertex>& angle, const Vec2& pPos, int off)
    {
        for (auto* vertices : light) 
        {
            delete[] vertices;
        }
        light.clear();  // Clear the vector to avoid dangling pointers
        for (int i = 0; i < angle.size() - off; i++)
        {
            if (static_cast<unsigned long long>(i) + 1 >= angle.size())
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
    void addLight(sf::VertexArray& light, const std::vector<sf::Vertex>& angle, const Vec2& pPos, int off)
    {
        light.clear();
        light = sf::VertexArray(sf::TriangleFan, angle.size() + 1 + off);
        light[0] = sf::Vertex(sf::Vector2f(pPos.x, pPos.y), sf::Color::White);

        for (int i = 1; i <= angle.size(); i++)
        {
            light[i] = angle[i - 1];
        }

        if(off > 0 ) light[angle.size() + 1] = light[1];
    }

    void addRay(std::vector<sf::Vertex*>& ray, const std::vector<sf::Vertex>& angle, const Vec2& pPos)
    {
        for (auto* vertices : ray) {
            delete[] vertices;
        }
        ray.clear();  // Clear the vector to avoid dangling pointers
        for (const auto& e : angle)
        {
            ray.push_back(
                new sf::Vertex[2]{
                sf::Vertex(sf::Vector2f(pPos.x, pPos.y), sf::Color::Red),
                sf::Vertex(sf::Vector2f(e.position.x, e.position.y), sf::Color::Red),
                }
                );
        }
    }


    Vec2 getOverlap(const std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& object) const
    {
        auto& eT = entity->getComponent<CTransform>();
        auto& eSize = entity->getComponent<CBoundingBox>().halfSize;
        auto& ePos = eT.pos;

        auto& oT = object->getComponent<CTransform>();
        auto& oSize = object->getComponent<CBoundingBox>().halfSize;
        auto& oPos = oT.pos;

        auto dx = abs(oPos.x - ePos.x);
        auto dy = abs(oPos.y - ePos.y);


        if (dx <= eSize.x + oSize.x || dy <= eSize.y + oSize.y)
        {
            return Vec2((eSize.x + oSize.x) - dx, (eSize.y + oSize.y) - dy);
        }
        return Vec2(0, 0);
    }

    Vec2 getPrevOverlap(const std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& object) const
    {
        auto& eT = entity->getComponent<CTransform>();
        auto& eSize = entity->getComponent<CBoundingBox>().halfSize;
        auto& ePos = eT.prevPos;


        auto& oT = object->getComponent<CTransform>();
        auto& oSize = object->getComponent<CBoundingBox>().halfSize;
        auto& oPos = oT.prevPos;


        auto dx = abs(ePos.x - oPos.x);
        auto dy = abs(ePos.y - oPos.y);


        if (dx <= eSize.x + oSize.x || dy <= eSize.y + oSize.y)
        {
            return Vec2((eSize.x + oSize.x) - dx, (eSize.y + oSize.y) - dy);
        }
        return Vec2(0, 0);
    }

    bool isInside(const sf::Vector2f& pos, const std::shared_ptr<Entity> e)
    {
        auto& ePos = e->getComponent<CTransform>().pos;
        auto& cb = e->getComponent<CBoundingBox>().size;

        Vec2 rad(cb.x, cb.y);

        float distX = fabs(pos.x - ePos.x);
        float distY = fabs(pos.y - ePos.y);
        return (distX < cb.x / 2 && distY < cb.y / 2);
    }

    const Vec2& getRectGrid(const Vec2& pos, const int rectSize, bool gridToVector)
    {
        int halfRect = rectSize / 2;
        if (gridToVector)
        {
            auto result = Vec2((pos.x * rectSize) + halfRect, (pos.y * rectSize) + halfRect);
            //std::cout << result.x << " " << result.y << std::endl;
            //std::cout << pos.x << " " << pos.y << std::endl;
            return result;
        }

        return Vec2((int)(pos.x / rectSize), (int)(pos.y / rectSize));
    }

    bool spotLight(std::vector<sf::Vertex>& angle, const sf::Vector2f& pPos, const std::shared_ptr<Entity> entities)
    {
        for (auto& e : angle)
        {
                const auto& f = entities->getComponent<CVertex>().vertex;
                if (isIntersect(pPos, e, f.at(0).position, f.at(1).position) ||
                    isIntersect(pPos, e, f.at(1).position, f.at(2).position) ||
                    isIntersect(pPos, e, f.at(2).position, f.at(3).position) ||
                    isIntersect(pPos, e, f.at(3).position, f.at(0).position)) return true;

        }
        return false;
    }

   
};

