#include "actions.hpp"
#include "behaviortree_cpp/bt_factory.h"
#include <ament_index_cpp/get_package_share_directory.hpp>

int main(){
    BT::BehaviorTreeFactory factory;
    factory.registerNodeType<Init>("Init");
    factory.registerNodeType<CheckExpr>("CheckExpr");
    
    try {
        const std::string pkg_share =
            ament_index_cpp::get_package_share_directory("rtdl");
        const std::string xml_path = pkg_share + "/trees/tree.xml";

        auto tree = factory.createTreeFromFile(xml_path);
        tree.tickRootWhileRunning();
    } catch (const std::exception& e) {
        std::cerr << "Failed: " << e.what() << '\n';
        return 1;
    }
}
