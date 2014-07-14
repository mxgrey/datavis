#ifndef DRAWLAYER_HPP
#define DRAWLAYER_HPP

#include <datavis/Surf3D.hpp>

namespace datavis {

class DrawLayer : public Surf3D
{
public:
    
    DrawLayer();
    DrawLayer(const std::string& filename, osg::Vec4 startColor = osg::Vec4(0,0,1,1),
                                           osg::Vec4 finalColor = osg::Vec4(1,0,0,1));

    void draw(std::vector<size_t> layers);
    
protected:
    
    virtual ~DrawLayer();
};


} // namespace datavis




#endif // DRAWLAYER_HPP
