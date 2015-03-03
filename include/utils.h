#ifndef UTILS_H
#define UTILS_H

//#include "siftfast/siftfast.h"
#include "include/ANN/ANN.h"

// system
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <limits>

// opencv and pcl
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/console/time.h>
#include <pcl/common/common_headers.h>
#include <pcl_conversions/pcl_conversions.h>

// eigen
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace Eigen;

typedef pcl::PointXYZRGB PointType;

// alternative solution to boost foreach
#define foreach( i, c ) for( typeof((c).begin()) i##_hid=(c).begin(), *i##_hid2=((typeof((c).begin())*)1); i##_hid2 && i##_hid!=(c).end(); ++i##_hid) for( typeof( *(c).begin() ) &i=*i##_hid, *i##_hid3=(typeof( *(c).begin() )*)(i##_hid2=NULL); !i##_hid3 ; ++i##_hid3, ++i##_hid2)

#define eforeach( i, it, c ) for( typeof((c).begin()) it=(c).begin(), i##_hid = (c).begin(), *i##_hid2=((typeof((c).begin())*)1); i##_hid2 && it!=(c).end(); (it==i##_hid)?++it,++i##_hid:i##_hid=it) for( typeof(*(c).begin()) &i=*it, *i##_hid3=(typeof( *(c).begin() )*)(i##_hid2=NULL); !i##_hid3 ; ++i##_hid3, ++i##_hid2)

inline Vector3f string_to_vector3f( string str );
inline vector<float> string_to_vector( string str );

const string models_dir = "";


/** object model */
class Model {
private:

public:
    struct Feature{
        Vector3f coord3d;
        Vector3f color;
        vector<float> descrip;
    };
    typedef vector<Feature> Features;

    enum object_type{ RIGID_SMALL, RIGID_BIG, OTHERS };
    object_type type_;
    string name_;
    Features features_;
    Vector3f bounding_box_[2];

    // constructor
    Model();
    ~Model();
    // load models
    void load_model( string filename );

    // comparator
    bool operator < ( const Model & model ) const;
};
typedef boost::shared_ptr<Model> SP_Model;

/** estimated pose */
struct Pose {
    Quaternionf     q_;
    Translation3f   t_;
    Matrix3f        r_;

    Pose();
    Pose( Quaternionf q, Translation3f t );
    friend ostream & operator <<( ostream & out, const Pose & pose );
};

/** transformation matrix */
struct TransformMatrix {
    Matrix4f p_;
    void init( const Quaternionf & q, const Translation3f & t );
    void init( const Pose & pose );
    Vector3f transform( Vector3f orig );
    Vector3f inverse_transform( Vector3f orig );
    friend ostream & operator <<( ostream & out, const TransformMatrix & tm );
};

/** recognised object */
struct Object {
    SP_Model model_;
    Pose pose_;
    Matrix4f homo_;
    float score_;

    vector<cv::Point> get_object_hull( vector<cv::Point> pts );
    bool operator < ( const Object & o ) const;
    friend ostream & operator << ( ostream & out, const Object & obj );
};
typedef boost::shared_ptr<Object> SP_Object;
bool compare_sp_object(const SP_Object &spo_l, const SP_Object &spo_r);

/** detected features */
struct DetectedFeatureRGBD{
    Vector2f    img2d;
    Vector3f    obs3d;
    int         groupidx;   // cluster index
    vector<float> descrip;
};

struct MatchRGBD{
    Vector2f    img2d;
    Vector3f    obs3d;
    Vector3f    mdl3d;
};

/** detected features and matches for rgb data */
struct DetectedFeatureRGB{
    Vector2f    img2d;
    vector<float> descrip;
};

struct MatchRGB{
    Vector2f    img2d;
    Vector3f    mdl3d;
};

// functions
cv::Point2f project( const Pose &pose, const pcl::PointXYZ & pt3d, const Vector4f & params );

Vector2f project( const Pose & pose, const Vector3f & vec3d, const Vector4f & params );

// functions for each step, RGBD
/** sift feature extraction using libsiftfast
  * @param rgb image
  * @param point cloud
  * @return detected features
  */
void display_features( const cv::Mat & image, const vector<DetectedFeatureRGBD> & features, bool clustered_flag );

/** rgbd segmentation
  * @param rgb point cloud
  * @param detected features
  * @param cluster
  * @return labeled point cloud
  */
//pcl::PointCloud<pcl::PointXYZRGBL>::Ptr segment_point_cloud( pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud, vector<DetectedFeatureRGBD> & detected_features, vector< vector<int> > & clusters );
void display_labeled_cloud( pcl::PointCloud<pcl::PointXYZRGBL>::Ptr labeled_cloud, int label );

/** match descriptors
  * @param detected features
  * @param models
  * @param cluster matches indices
  * @return matches
  */
void display_matches(const cv::Mat &image, const vector<MatchRGBD> &matches );

// display rgbd matches
void display_clusters(const cv::Mat &image, const vector<MatchRGBD> &matches, vector<list<int> > &clusters);

/** pose estimation using svd
  * @param matches
  * @param clusters
  * @param models
  * @return objects
  */
const float g_pose_svd_error  = 0.005;
const int   g_pose_svd_minpts = 7;
//list<SP_Object> svd_pose_estimation_recog( const vector< vector<MatchRGBD> > & matches, const vector<SP_Model> & models, const vector< vector<int> > & clusters );
//pair<float, int> svd_pose_estimation_core( vector<MatchRGBD> & matches, Pose & pose );
//bool svd_pose_estimation_veri( vector<MatchRGBD> & matches, Pose & pose );
void display_pose(const cv::Mat &image, const list<SP_Object> &objects, const Vector4f & cp);

// functions for each step, RGB recogniser

/** ************************************************************************* */
// display mask image
void display_mask( const cv::Mat & mask_image );

//vector<DetectedFeatureRGB> detect_features_rgb( const cv::Mat & image );
void display_features( const cv::Mat & image, const vector< DetectedFeatureRGB > & features );


// match rgb features
void display_matches(const cv::Mat &image, const vector<MatchRGB> &matches );


// display cluster features
void display_clusters( const cv::Mat & image, const vector<MatchRGB> & matches, vector< list<int> > & clusters );




#endif // UTILS_H
