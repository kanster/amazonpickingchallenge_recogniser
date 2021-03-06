/** test for kernel descriptor recognition */

#include "include/helpfun/kd_recogniser.h"

#include <pcl/visualization/pcl_visualizer.h>


int main( int argc, char ** argv ) {
    // observation
    cv::Mat rgb_image = cv::imread( string(argv[1]), CV_LOAD_IMAGE_COLOR );
    cv::Mat depth_image = cv::imread( string(argv[2]), CV_LOAD_IMAGE_ANYDEPTH );
//    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud( new pcl::PointCloud<pcl::PointXYZRGB>() );
//    pcl::io::loadPCDFile( data_folder+string(argv[3])/*"cloud5.pcd"*/, *cloud );

    KDRecogniser kdr;
    kdr.load_sensor_data( rgb_image, depth_image);

    // load pre collected information
    cv::Mat empty_image = cv::imread( argv[3], CV_LOAD_IMAGE_COLOR );
    cv::Mat mask_image = cv::imread( argv[4], CV_LOAD_IMAGE_GRAYSCALE );
    cv::Mat empty_depth_image = cv::imread( argv[5], CV_LOAD_IMAGE_ANYDEPTH );
    kdr.load_info( empty_image, mask_image, empty_depth_image );

    // init libkdes
    string svm_model_name = "modelrgbkdes";
    string kdes_model_name = "kpcaRGBDes";
    string model_folder = "/home/kanzhi/code/kdes/models/";
    string model = "RGB Kernel Descriptor";
    unsigned int model_type = 2;
    kdr.init_libkdes( svm_model_name, kdes_model_name, model_folder, model, model_type );


    // environment settings
    vector<string> items;
    for ( int i = 6; i < argc; ++ i )
        items.push_back( argv[i] );
    kdr.set_env_configuration( argv[6], items );



    vector<pair<string, vector<cv::Point> > > results = kdr.process();

    for ( int i = 0; i < (int)results.size(); ++ i ) {
        cv::putText( rgb_image, results[i].first, results[i].second[0], CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 0.5, cv::Scalar(0,0,255));
        for ( int j = 0; j < (int)results[i].second.size(); ++ j )
            cv::line( rgb_image, results[i].second[j], results[i].second[(j+1)%results[i].second.size()], cv::Scalar(255, 0, 255), 2 );
    }

    cv::imshow( "rgb_image", rgb_image );
    cv::waitKey(0);

//    vector<pcl::PointCloud<pcl::PointXYZRGB>::Ptr > rect_clouds = kdr.get_rect_clouds();
//    pcl::visualization::PCLVisualizer::Ptr viewer( new pcl::visualization::PCLVisualizer("rect_cloud") );
//    viewer->setBackgroundColor (0, 0, 0);
//    for ( int i = 0; i < (int)rect_clouds.size(); ++ i ) {
//        pcl::PointCloud<pcl::PointXYZRGB>::Ptr rect_cloud = rect_clouds[i];
//        pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(rect_cloud);
//        viewer->addPointCloud<pcl::PointXYZRGB> (rect_cloud, rgb, results[i].first);
//        viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, results[i].first);
//    }

//    viewer->addCoordinateSystem(0.5);
//    viewer->initCameraParameters();
//    while (!viewer->wasStopped ()) {
//        viewer->spinOnce(100);
//        boost::this_thread::sleep(boost::posix_time::microseconds (100000));
//    }

    return 0;


}
