#ifndef KDTREE2D_H
#define KDTREE2D_H
#include <cstddef>
#include <vector>

namespace AirRoute {
namespace kdtree2d {
    struct Point2D {
        std::size_t index;
        double x;  
        double y;
    };

    class KDTree2D {
        public:
            KDTree2D() = default;
            ~KDTree2D();
            void build(const std::vector<Point2D>& points);
            void clear();
            bool nearest(double query_x, double query_y, std::size_t& out_index) const;
            void within_radius_m(double query_x, double query_y, double radius_m, std::vector<std::size_t>& out_indices) const;

        private:
            struct Node2D {
                std::size_t idx;
                double x;
                double y;
                Node2D* left;
                Node2D* right;
            };
            Node2D* root_ = nullptr;
            static Node2D* create_node(std::size_t idx, double x, double y);
            static void destroy_tree(Node2D* node);
            static Node2D* insert(Node2D* root, std::size_t idx, double x, double y, bool axis_x);
            static Node2D* nearest_node(Node2D* root, double qx, double qy, bool axis_x);
            static void radius_collect(Node2D* root, double qx, double qy, double radius_dist_sq, bool axis_x, std::vector<std::size_t>& out);
            static double dist_euclid_sq(double x1, double y1, double x2, double y2);
            static Node2D* closer(double qx, double qy, Node2D* a, Node2D* b);
    };

}  // namespace kdtree2d
}  // namespace AirRoute

#endif
