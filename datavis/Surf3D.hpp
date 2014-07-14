#ifndef SURF3D_HPP
#define SURF3D_HPP

#include <osg/Geode>
#include <osgText/Text>

#include <string>
#include <vector>

namespace datavis {

class Surf3D : public osg::Geode
{
public:

    Surf3D();
    Surf3D(const std::string& filename, osg::Vec4 startColor = osg::Vec4(0,0,1,1),
                                        osg::Vec4 finalColor = osg::Vec4(1,0,0,1),
           bool autoAssemble=true);
    
    double x_scale;
    double y_scale;
    double z_scale;
    double upper_axis_scale;
    double lower_axis_scale;

    std::vector< std::vector<double> > data;

    std::vector< std::vector< std::vector<unsigned int> > > p;

    osg::Vec4 startColor;
    osg::Vec4 finalColor;
    osg::Vec4 getColor(size_t layer);

    void assemble();

    osg::Vec3 lower;
    osg::Vec3 upper;
    osg::Vec3 mid;

protected:
    
    virtual ~Surf3D();

    osg::ref_ptr<osg::Geometry> _geom;
    osg::ref_ptr<osg::Geometry> _axisGeom;
    osg::ref_ptr<osg::Vec3Array> _verts;
    osg::ref_ptr<osg::Vec3Array> _axisVerts;
    osg::ref_ptr<osg::Vec3Array> _normals;
    osg::ref_ptr<osg::Vec4Array> _colors;
    osg::ref_ptr<osg::Vec4Array> _axisColors;
    osg::ref_ptr<osg::DrawElementsUInt> _faces;
    osg::ref_ptr<osg::DrawElementsUInt> _axisLines;
    osg::ref_ptr<osg::UIntArray> _nbinds;
    
    osg::ref_ptr<osgText::Text> _x_text;
    osg::ref_ptr<osgText::Text> _y_text;
    osg::ref_ptr<osgText::Text> _z_text;
};


} // namespace datavis

#endif // SURF3D_HPP
