#include "kdtree2d.h"

#include <cmath>

namespace AirRoute {
namespace kdtree2d {
    KDTree2D::~KDTree2D() {
        clear();
    }

    void KDTree2D::clear() {
        destroy_tree(root_);
        root_ = nullptr;
    }

    void KDTree2D::build(const std::vector<Point2D>& points) { // O (n log n)
        clear();
        for (const Point2D& p : points) {
            root_ = insert(root_, p.index, p.x, p.y, true);
        }
    }

    KDTree2D::Node2D* KDTree2D::create_node(std::size_t idx, double x, double y) {
        Node2D* node = new Node2D();
        node->idx = idx;
        node->x = x;
        node->y = y;
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }

    void KDTree2D::destroy_tree(Node2D* node) {
        if (node == nullptr) return;
        destroy_tree(node->left);
        destroy_tree(node->right);
        delete node;
    }

    KDTree2D::Node2D* KDTree2D::insert(Node2D* root, std::size_t idx, double x, double y, bool axis_x) {
        if (root == nullptr) return create_node(idx, x, y);
        if (axis_x) {
            if (x < root->x) root->left = insert(root->left, idx, x, y, false);
            else root->right = insert(root->right, idx, x, y, false);
        } else {
            if (y < root->y) root->left = insert(root->left, idx, x, y, true);
            else root->right = insert(root->right, idx, x, y, true);
        }
        return root;
    }



    bool KDTree2D::nearest(double qx, double qy, std::size_t& out_index) const { // O (log n) 
        Node2D* best = nearest_node(root_, qx, qy, true);
        if (best == nullptr) return false;
        out_index = best->idx;
        return true;
    }

    void KDTree2D::within_radius_m(double qx, double qy, double radius_m, std::vector<std::size_t>& out_indices) const {
        out_indices.clear();
        if (radius_m < 0.0) return;
        const double radius_dist_sq = radius_m * radius_m;
        radius_collect(root_, qx, qy, radius_dist_sq, true, out_indices);
    }


    void KDTree2D::radius_collect(Node2D* root, double qx, double qy, double radius_dist_sq, bool axis_x, std::vector<std::size_t>& out) {
        if (root == nullptr) return;
        if (dist_euclid_sq(qx, qy, root->x, root->y) <= radius_dist_sq) {
            out.push_back(root->idx);
        }
        Node2D* near_side = nullptr;
        Node2D* far_side = nullptr;
        double dist_to_plane_sq = 0.0;
        if (axis_x) {
            if (qx < root->x) near_side = root->left, far_side = root->right;
            else near_side = root->right, far_side = root->left;
            dist_to_plane_sq = (qx - root->x) * (qx - root->x);   
        } else {
            if (qy < root->y) near_side = root->left, far_side = root->right;
            else near_side = root->right, far_side = root->left;
            dist_to_plane_sq = (qy - root->y) * (qy - root->y);
        }
        radius_collect(near_side, qx, qy, radius_dist_sq, !axis_x, out);
        if (dist_to_plane_sq <= radius_dist_sq) {
            radius_collect(far_side, qx, qy, radius_dist_sq, !axis_x, out);
        }
    }



    double KDTree2D::dist_euclid_sq(double x1, double y1, double x2, double y2) {
        return (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2);
    }


    KDTree2D::Node2D* KDTree2D::closer(double qx, double qy, Node2D* a, Node2D* b) {
        if (a == nullptr) return b;
        if (b == nullptr) return a;
        return dist_euclid_sq(qx, qy, a->x, a->y) < dist_euclid_sq(qx, qy, b->x, b->y) ? a : b;
    }

    KDTree2D::Node2D* KDTree2D::nearest_node(Node2D* root, double qx, double qy, bool axis_x) {
        if (root == nullptr) return nullptr;
        Node2D* near_side = nullptr;
        Node2D* far_side = nullptr;
        double dist_to_plane_sq = 0.0;
        if (axis_x) {
            if (qx < root->x) near_side = root->left, far_side = root->right;
            else near_side = root->right, far_side = root->left;
            dist_to_plane_sq = (qx - root->x) * (qx - root->x);
        } else {
            if (qy < root->y) near_side = root->left, far_side = root->right;
            else near_side = root->right, far_side = root->left;
            dist_to_plane_sq = (qy - root->y) * (qy - root->y);
        }

        Node2D* tmp = nearest_node(near_side, qx, qy, !axis_x);
        Node2D* best = closer(qx, qy, tmp, root);
        const double best_dist_sq = dist_euclid_sq(qx, qy, best->x, best->y);
        if (dist_to_plane_sq <= best_dist_sq) {
            tmp = nearest_node(far_side, qx, qy, !axis_x);
            best = closer(qx, qy, best, tmp);
        }
        return best;
    }

}  // namespace kdtree2d
}  // namespace AirRoute
