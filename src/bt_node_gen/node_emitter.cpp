#include "bt_node_gen/node_emitter.hpp"
#include "contexts/backend_context.hpp"
#include "contexts/frontend_context.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <assert.h>

extern FrontendContext& fc;
extern BackendContext& bc;
namespace fs = std::filesystem;

static std::string getClassName(SkillSymSPtr skiSym){
    return skiSym->getName() + "Node";
}

static std::string lowerStr(const std::string& ros2type){
    std::string ans;
    int cnt = 0;
    for(char ch: ros2type){
        if(ch >= 'A' && ch <= 'Z'){
            ch = ch - 'A' + 'a';
            if(cnt++) ans.push_back('_');
        } 
        ans.push_back(ch);
    }
    return ans;
}  

std::string NodeEmitter::cppTypeOf(const std::string& ros2type){
    if(ros2type == "string") return "std::string";
    std::string ans;
    for(const auto ch: ros2type){
        if(ch != '/') ans.push_back(ch);
        else ans += "::";
    }
    return ans;
}

std::string NodeEmitter::toReqAccExpr(const std::string& path){
    const std::string prefix = "request.";
    if(path.rfind(prefix, 0) != 0){
        throw std::runtime_error("target must start with request.: " + path);
    }
    std::string suffix = path.substr(prefix.size());
    return "request->" + suffix;
}
std::string NodeEmitter::toResponseAccExpr(const std::string& path){
    const std::string prefix = "response.";
    if(path.rfind(prefix, 0) != 0){
        throw std::runtime_error("target must start with response.: " + path);
    }
    std::string suffix = path.substr(prefix.size());
    return "response->" + suffix;
}
std::optional<std::string> NodeEmitter::findSuccessSrc(SkillSymSPtr sym){
    for(const auto& m: sym->getOutputToROSMappings()){
        if(m.port == "success"){
            return toResponseAccExpr(m.source);
        }
    }
    return std::nullopt;
}
void NodeEmitter::printWithIndent(const std::string& content, std::ostream& o, int indent){
    o << std::string(indent, ' ') << content;
}

void NodeEmitter::emitHPPHeader(SkillSymSPtr skiSym){
    std::ostream& hpp_out = bc.getHPPOut();
    std::string tmp = skiSym->getROS2Backend();
    if(tmp == "action"){
        printWithIndent("#include <behaviortree_ros2/bt_action_node.hpp>\n", hpp_out, 0);
    } else if(tmp == "service"){
        printWithIndent("#include <behaviortree_ros2/bt_service_node.hpp>\n", hpp_out, 0);
    }
    std::string ros2type = skiSym->getROS2ReqType();
    if(!ros2type.empty()) printWithIndent("#include <" + lowerStr(ros2type) + ".hpp>\n", hpp_out, 0);
}
void NodeEmitter::emitCPPHeader(SkillSymSPtr skiSym){
    std::ostream& cpp_out = bc.getCPPOut();
    std::string hClassName = lowerStr(skiSym->getName());
    printWithIndent("#include <" + bc.ros2Pkg + "/" + hClassName + ".hpp>\n", cpp_out, 0);
}
void NodeEmitter::emitHPPClass(SkillSymSPtr skiSym){
    std::ostream& hpp_out = bc.getHPPOut(), &cpp_out = bc.getCPPOut();

    std::string baseClassName = "BT::";
    std::string tmp = skiSym->getROS2Backend();
    bool isSrv = tmp == "service", isAct = tmp == "action";
    assert(isSrv && !isAct || !isSrv && isAct);

    if(isSrv){
        baseClassName += "RosServiceNode";
    } else if(isAct){
        baseClassName += "RosActionNode";
    }
    baseClassName += "<" + bc.ReqTypeInCPP + ">";
    std::string classDef = "class " + bc.className + ": public " + baseClassName + "\n";

    printWithIndent(classDef, hpp_out, 0);
    printWithIndent("{\npublic:\n", hpp_out, 0);

    if(isSrv){
        printWithIndent("using ServiceType = " + bc.ReqTypeInCPP + ";\n", hpp_out, 4);
        printWithIndent("using Base = BT::RosServiceNode<ServiceType>;\n", hpp_out, 4);
        printWithIndent("using Request = typename ServiceType::Request;\n", hpp_out, 4);
        printWithIndent("using Response = typename ServiceType::Response;\n", hpp_out, 4);
    } else if(isAct){
        printWithIndent("using ActionType = " + bc.ReqTypeInCPP + ";\n", hpp_out, 4);
        printWithIndent("using Base = BT::RosActionNode<ActionType>;\n", hpp_out, 4);
        printWithIndent("using Goal = typename ActionType::Goal;\n", hpp_out, 4);
        printWithIndent("using WrappedResult = typename rclcpp_action::ClientGoalHandle<ActionType>::WrappedResult;\n", hpp_out, 4);
    }

    emitHPPConstruct(skiSym);
    emitHPPPorts(skiSym);
    if(isAct){
        emitHPPSetGoal(skiSym);
        emitHPPOnResultReceived(skiSym);
    } else if(isSrv){
        emitHPPSetReq(skiSym);
        emitHPPOnResponseReceived(skiSym);
    }
    emitHPPOnFailure(skiSym);
    
    printWithIndent("};\n", hpp_out,0);
}
void NodeEmitter::emitCPPClass(SkillSymSPtr skiSym){
    emitCPPConstruct(skiSym);
    emitCPPPorts(skiSym);
    std::string tmp = skiSym->getROS2Backend();
    if(tmp == "action"){
        emitCPPSetGoal(skiSym);
        emitCPPOnResultReceived(skiSym);
    } else if(tmp == "service"){
        emitCPPSetReq(skiSym);
        emitCPPOnResponseReceived(skiSym);
    }
    emitCPPOnFailure(skiSym);
}

void NodeEmitter::emitHPPConstruct(SkillSymSPtr skiSym){
    std::ostream& hpp_out = bc.getHPPOut();
    printWithIndent(bc.className + "(const std::string& name, const BT::NodeConfig& conf, const BT::RosNodeParams& params);\n", hpp_out, 4);
}
void NodeEmitter::emitCPPConstruct(SkillSymSPtr skiSym){
    std::ostream& cpp_out = bc.getCPPOut();
    printWithIndent(bc.className + "::" + bc.className + "(const std::string& name, const BT::NodeConfig& conf, const BT::RosNodeParams& params) : Base(name, conf, params){}\n", cpp_out, 0);
}
void NodeEmitter::emitHPPPorts(SkillSymSPtr skiSym){
    std::ostream& hpp_out = bc.getHPPOut();
    printWithIndent("static BT::PortsList providedPorts();\n", hpp_out, 4);
}
void NodeEmitter::emitCPPPorts(SkillSymSPtr skiSym){
    std::ostream& cpp_out = bc.getCPPOut();
    printWithIndent("BT::PortsList " + bc.className + "::providedPorts()\n{\n", cpp_out, 0);
    printWithIndent("return providedBasicPorts({\n", cpp_out, 4);

    const auto& inParas = skiSym->getInParas();
    const auto& outParas = skiSym->getOutParas();
    for(const auto& it: inParas){
        std::string name = it.first, type = it.second;
        type = cppTypeOf(type);
        printWithIndent("BT::InputPort<" + type + ">(\"" + name + "\"),\n", cpp_out, 8);
    }
    for(const auto& it: outParas){
        std::string name = it.first, type = it.second;
        type = cppTypeOf(type);
        printWithIndent("BT::OutputPort<" + type + ">(\"" + name + "\"),\n", cpp_out, 8);
    }
    printWithIndent("});\n", cpp_out, 4);
    printWithIndent("}\n", cpp_out, 0);
}
void NodeEmitter::emitHPPOnFailure(SkillSymSPtr skiSym){
    std::ostream& hpp_out = bc.getHPPOut();
    std::string tmp = skiSym->getROS2Backend();
    if(tmp == "service"){
        printWithIndent("BT::NodeStatus onFailure(BT::ServiceNodeErrorCode err);\n", hpp_out, 4);
    } else if(tmp == "action"){
        printWithIndent("BT::NodeStatus onFailure(BT::ActionNodeErrorCode err);\n", hpp_out, 4);
    }
}
void NodeEmitter::emitCPPOnFailure(SkillSymSPtr skiSym){
    std::ostream& cpp_out = bc.getCPPOut();
    std::string tmp = skiSym->getROS2Backend();
    if(tmp == "service"){
        printWithIndent("BT::NodeStatus " + bc.className + "::onFailure(BT::ServiceNodeErrorCode err)\n{\n", cpp_out, 0);
    } else if(tmp == "action"){
        printWithIndent("BT::NodeStatus " + bc.className + "::onFailure(BT::ActionNodeErrorCode err)\n{\n", cpp_out, 0);
    }
    printWithIndent("return BT::NodeStatus::FAILURE;\n", cpp_out, 4);
    printWithIndent("}\n", cpp_out, 0);
}
// action
void NodeEmitter::emitHPPSetGoal(SkillSymSPtr skiSym){
    std::ostream& hpp_out = bc.getHPPOut();
    printWithIndent("bool setGoal(Goal& goal);\n", hpp_out, 4);
}
//  TODO: need schema.
void NodeEmitter::emitCPPSetGoal(SkillSymSPtr skiSym){
    std::ostream& cpp_out = bc.getCPPOut();
    assert(skiSym->getROS2Backend() == "action");
}
void NodeEmitter::emitHPPOnResultReceived(SkillSymSPtr skiSym){
    std::ostream& hpp_out = bc.getHPPOut();
    printWithIndent("BT::NodeStatus onResultReceived(const WrappedResult& wr);\n", hpp_out, 4);
}
// TODO: need schema.
void NodeEmitter::emitCPPOnResultReceived(SkillSymSPtr skiSym){
    std::ostream& cpp_out = bc.getCPPOut();
    assert(skiSym->getROS2Backend() == "action");
}
// service
void NodeEmitter::emitHPPSetReq(SkillSymSPtr skiSym){
    std::ostream& hpp_out = bc.getHPPOut();
    printWithIndent("bool setRequest(typename Request::SharedPtr& req);\n", hpp_out, 4);
}
void NodeEmitter::emitCPPSetReq(SkillSymSPtr skiSym){
    std::ostream& cpp_out = bc.getCPPOut();
    assert(skiSym->getROS2Backend() == "service");
    printWithIndent("bool " + bc.className + "::setRequest(Request::SharedPtr& request)\n{\n", cpp_out, 0);
    const auto& inMps = skiSym->getInputToROSMappings();
    for(const auto& inMp: inMps){
        std::string portName = inMp.port, tarExpr = toReqAccExpr(inMp.target);
        std::string localType = cppTypeOf(skiSym->getInType(portName));
        printWithIndent(localType + " " + portName + ";\n", cpp_out, 4);
        printWithIndent("if(!getInput(\"" + portName + "\", " + portName + ")){ return false; }\n", cpp_out, 4);
        printWithIndent(tarExpr + " = " + portName + ";\n", cpp_out, 4);

    }
    printWithIndent("return true;\n", cpp_out, 4);
    printWithIndent("}\n", cpp_out, 0);
}
void NodeEmitter::emitHPPOnResponseReceived(SkillSymSPtr skiSym){
    std::ostream& hpp_out = bc.getHPPOut();
    printWithIndent("BT::NodeStatus onResponseReceived(const typename Response::SharedPtr& res);\n", hpp_out, 4);
}
void NodeEmitter::emitCPPOnResponseReceived(SkillSymSPtr skiSym){
    std::ostream& cpp_out = bc.getCPPOut();
    assert(skiSym->getROS2Backend() == "service");
    const auto& outMps = skiSym->getOutputToROSMappings();
    printWithIndent("BT::NodeStatus " + bc.className + "::onResponseReceived(const Response::SharedPtr& response)\n{\n", cpp_out, 0);
    for(const auto& outMp: outMps){
        std::string portName = outMp.port, srcExpr = toResponseAccExpr(outMp.source);
        printWithIndent("setOutput(\"" + portName + "\", " + srcExpr + ");\n", cpp_out, 4);
    }
    auto sucExpr = findSuccessSrc(skiSym);
    if(sucExpr){
        printWithIndent("return " + *sucExpr + "? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;\n", cpp_out, 4);
    } else {
        printWithIndent("return BT::NodeStatus::SUCCESS;\n", cpp_out, 4);
    }
    printWithIndent("}\n", cpp_out, 0);
}

void NodeEmitter::emitHPP(SkillSymSPtr skiSym){
    std::string name = skiSym->getName(); name = lowerStr(name);
    fs::path filePath = bc.ros2Pkg + "/include/" + bc.ros2Pkg + "/" + name + ".hpp";
    if(!filePath.parent_path().empty()){
        fs::create_directories(filePath.parent_path());
    }
    std::ofstream hpp_out(filePath);
    if(!hpp_out.is_open()){
        throw std::runtime_error("create hpp output stream failed.");
    }
    bc.hpp_out_ = &hpp_out;

    emitHPPHeader(skiSym);
    emitHPPClass(skiSym);

    bc.hpp_out_ = nullptr;
}
void NodeEmitter::emitCPP(SkillSymSPtr skiSym){
    std::string name = skiSym->getName(); name = lowerStr(name);
    fs::path filePath = bc.ros2Pkg + "/src/" + name + ".cpp";
    if(!filePath.parent_path().empty()){
        fs::create_directories(filePath.parent_path());
    }
    std::ofstream cpp_out(filePath);
    if(!cpp_out.is_open()){
        throw std::runtime_error("create hpp output stream failed.");
    }
    bc.cpp_out_ = &cpp_out;

    emitCPPHeader(skiSym);
    emitCPPClass(skiSym);

    bc.cpp_out_ = nullptr;
}

void NodeEmitter::emitMainHeader(std::ostream& o){
    printWithIndent("#include <memory>\n\
#include <cstdlib>\n\
#include <string>\n\
#include <fstream>\n\
#include <iostream>\n\
#include \"ament_index_cpp/get_package_share_directory.hpp\"\n\
#include \"behaviortree_cpp/bt_factory.h\"\n\
#include \"behaviortree_cpp/xml_parsing.h\"\n\
#include \"rclcpp/rclcpp.hpp\"\n",
        o,
        0
    );
    auto globSymMp = fc.globTable->getAllSymbols();
    for(const auto& it: globSymMp){
        if(it.second->getSE() == SYM_ENUM::SKILL_SYM){
            SkillSymSPtr skiSym = std::dynamic_pointer_cast<SkillSym>(it.second);
            std::string includeName = bc.ros2Pkg + "/" + lowerStr(skiSym->getName());
            printWithIndent("#include \"" + includeName + ".hpp\"\n", o, 0);
        }
    }
}
void NodeEmitter::emitMainArgParse(std::ostream& o){
    std::string content("struct Param{\n\
    std::string key;\n\
    std::string value;\n\
};\n\
void handleParas(int argc, char** argv, std::vector<Param>& paras){\n\
    std::cout << \"argc: \" << argc << '\\n';\n\
    for(int i = 1; i < argc; i += 2){\n\
        for(auto& it: paras){\n\
            if(argv[i] == it.key){\n\
                if(i + 1 >= argc){\n\
                    throw std::runtime_error(\"Param Error: lack value of \" + it.key);\n\
                }\n\
                it.value = argv[i + 1];\n\
            }\n\
        }\n\
    }\n\
}\n");
    o << content;
}
static void emitFileOpsFunc(std::ostream& o){
    std::string file_read = "static std::string readWholeFile(const std::string& path)\n\
{\n\
    std::ifstream fin(path);\n\
    if (!fin.is_open()) {\n\
        throw std::runtime_error(\"Failed to open file for reading: \" + path);\n\
    }\n\
    std::ostringstream ss;\n\
    ss << fin.rdbuf();\n\
    return ss.str();\n\
};\n";
    std::string file_write = "static void writeWholeFile(const std::string& path, const std::string& content)\n\
{\n\
    std::ofstream fout(path);\n\
    if (!fout.is_open()) {\n\
        throw std::runtime_error(\"Failed to open file for writing: \" + path);\n\
    }\n\
    fout << content;\n\
    if (!fout.good()) {\n\
        throw std::runtime_error(\"Failed to write file: \" + path);\n\
    }\n\
}\n";
    o << file_read << "\n" << file_write;
}
void NodeEmitter::emitFacReg(std::ostream& o){
    printWithIndent("static void registerExecutionNodes(BT::BehaviorTreeFactory& factory, const rclcpp::Node::SharedPtr& ros_node)\n{\n", o, 0);
    auto globSymMp = fc.globTable->getAllSymbols();
    for(const auto& it: globSymMp){
        if(it.second->getSE() == SYM_ENUM::SKILL_SYM){
            SkillSymSPtr skiSym = std::dynamic_pointer_cast<SkillSym>(it.second);
            printWithIndent("{\n", o, 4);
            printWithIndent("BT::RosNodeParams params;\n", o, 8);
            printWithIndent("params.nh = ros_node;\n", o, 8);
            printWithIndent("params.default_port_value = \"" + skiSym->getROS2Srv() + "\";\n", o, 8);
            printWithIndent("params.wait_for_server_timeout = std::chrono::milliseconds(3000);\n", o, 8); 
            printWithIndent("params.server_timeout = std::chrono::seconds(30);\n", o, 8);
            printWithIndent("factory.registerNodeType<" + skiSym->getName() + "Node>(\"" + skiSym->getName() + "\", params);\n", o, 8);
            printWithIndent("}\n", o, 4);
        }
    }
    printWithIndent("}\n", o, 0);
    printWithIndent("static void registerVisualizationNodes(BT::BehaviorTreeFactory& factory)\n{\n", o, 0);
    for(const auto& it: globSymMp){
        if(it.second->getSE() == SYM_ENUM::SKILL_SYM){
            SkillSymSPtr skiSym = std::dynamic_pointer_cast<SkillSym>(it.second);
            printWithIndent("{\n", o, 4);
            printWithIndent("BT::RosNodeParams params;\n", o, 8);
            printWithIndent("factory.registerNodeType<" + skiSym->getName() + "Node>(\"" + skiSym->getName() + "\", params);\n", o, 8);
            printWithIndent("}\n", o, 4);
        }
    }
    printWithIndent("}\n", o, 0);
}
static void emitHandleXML(std::ostream& o){
    std::string content = "static void handleXMLS(BT::BehaviorTreeFactory& factory){\n\
    const std::string pkg_share = ament_index_cpp::get_package_share_directory(\"rtdl_demo_bt_test\");\n\
    const std::string xml_file = pkg_share + \"/trees/plan.xml\";\n\
    std::cout << \"loading XML file of BT from: \" << xml_file << '\\n';\n\
    factory.registerBehaviorTreeFromFile(xml_file);\n\
    std::string xml_models = BT::writeTreeNodesModelXML(factory);\n\
    const std::string model_file = pkg_share + \"/trees/models.xml\";\n\
    std::cout << \"saving MODEL file of BT to: \" << model_file << '\\n';\n\
    std::ofstream model_out(model_file); \n\
    model_out << xml_models;\n\
}\n";
    o << content;
}
static void emitRun(std::ostream& o){
    std::string content = "static void run(int argc, char** argv){\n\
    rclcpp::init(argc, argv);\n\
    auto ros_node = rclcpp::Node::make_shared(\"bt_runner\");\n\
    BT::BehaviorTreeFactory factory;\n\n\
    BT::RosNodeParams params;\n\
    params.nh = ros_node;\n\
    registerExecutionNodes(factory, ros_node);\n\
    handleXMLS(factory);\n\n\
    auto tree = factory.createTree(\"Main\");\n\
    const auto status = tree.tickWhileRunning();\n\
    if (status == BT::NodeStatus::SUCCESS) { RCLCPP_INFO(ros_node->get_logger(), \"Behavior tree finished with SUCCESS.\");}\n\
    else if (status == BT::NodeStatus::FAILURE){ RCLCPP_ERROR(ros_node->get_logger(), \"Behavior tree finished with FAILURE.\");}\n\
    else {RCLCPP_WARN(ros_node->get_logger(), \"Behavior tree finished with RUNNING.\"); }\n\
    rclcpp::shutdown();\n\
}\n";
    o << content;
}
static void emitMergeStructAndModel(std::ostream& o){
    std::string handleModel = "static std::string extractTreeNodesModelBlock(const std::string& models_content)\n\
{\n\
    const std::string begin_tag = \"<TreeNodesModel>\";\n\
    const std::string end_tag = \"</TreeNodesModel>\";\n\
    std::size_t begin_pos = models_content.find(begin_tag);\n\
    if (begin_pos == std::string::npos) {\n\
        return models_content;\n\
    }\n\
    std::size_t end_pos = models_content.find(end_tag, begin_pos);\n\
    end_pos += end_tag.size();\n\
    return models_content.substr(begin_pos, end_pos - begin_pos);\n\
}\n";
    std::string mergeFunc = "static void mergePlanAndModelsToFile(const std::string& plan_xml_path,\n\
                                     const std::string& models_xml_path,\n\
                                     const std::string& output_xml_path)\n\
{\n\
    std::string plan_content = readWholeFile(plan_xml_path);\n\
    std::string models_content = readWholeFile(models_xml_path);\n\
    std::string tree_nodes_model = extractTreeNodesModelBlock(models_content);\n\
    const std::string root_end = \"</root>\";\n\
    std::size_t insert_pos = plan_content.rfind(root_end);\n\
    \n\
    std::string merged;\n\
    merged.reserve(plan_content.size() + tree_nodes_model.size() + 32);\n\
    merged.append(plan_content.substr(0, insert_pos));\n\
    merged.append(\"\\n\\n\");\n\
    merged.append(tree_nodes_model);\n\
    merged.append(\"\\n\\n\");\n\
    merged.append(plan_content.substr(insert_pos));\n\
    writeWholeFile(output_xml_path, merged);\n\
}\n";
    o << handleModel << "\n" << mergeFunc;
}
static void emitLaunchGroot2(std::ostream& o){
    std::string content = "static void launchGroot2(){\n\
    const std::string appImage(\"~/tools/Groot2-v1.9.0-x86_64.AppImage\");\n\
    const std::string pkg_share = ament_index_cpp::get_package_share_directory(\"rtdl_demo_bt_test\");\n\
    const std::string xml_file = pkg_share + \"/trees/plan.xml\";\n\
    const std::string model_file = pkg_share + \"/trees/models.xml\";\n\
    const std::string merged_file = pkg_share + \"/trees/for_groot2.xml\";\n\
    mergePlanAndModelsToFile(xml_file, model_file, merged_file);\n\
    std::cout << \"Launching Groot2 with merged XML file: \" << merged_file << '\\n';\n\
    std::string cmd =\n\
        appImage + \" --file \" + merged_file +\n\
        \" >/tmp/groot2.log 2>&1 &\";\n\
    std::cout << \"cmd: \"  << cmd << '\\n';\n\
    std::system(cmd.c_str());\n\
}\n";
    o << content;
}
static void emitVisualize(std::ostream& o){
    std::string content = "static void visualize(){\n\
    BT::BehaviorTreeFactory factory;\n\
    registerVisualizationNodes(factory);\n\
    handleXMLS(factory);\n\
    launchGroot2();\n\
}\n";
    o << content;
}
void NodeEmitter::emitMainBody(std::ostream& o){
    std::string content = "int main(int argc, char** argv)\n\
{\n\
    Param mode{\"--mode\", \"\"};\n\
    std::vector<Param> paras({mode});\n\
    handleParas(argc, argv, paras);\n\n\
    if(paras[0].value == \"run\"){\n\
        run(argc, argv);\n\
    } else if(paras[0].value == \"visualize\"){\n\
        visualize();\n\
    }\n\
    return 0;\n\
}\n";
    o << content;
}

void NodeEmitter::emitBTRunner(){
    fs::path filePath = bc.ros2Pkg + "/src/" + "bt_runner.cpp";
    if(!filePath.parent_path().empty()){
        fs::create_directories(filePath.parent_path());
    }
    std::ofstream o(filePath);
    
    emitMainHeader(o);
    emitMainArgParse(o);
    emitFileOpsFunc(o);
    emitFacReg(o);
    emitHandleXML(o);
    emitRun(o);
    emitMergeStructAndModel(o);
    emitLaunchGroot2(o);
    emitVisualize(o);
    emitMainBody(o);
}

void NodeEmitter::emitNode(){
    auto globSymMp = fc.globTable->getAllSymbols();
    for(const auto& it: globSymMp){
        if(it.second->getSE() == SYM_ENUM::SKILL_SYM){
            SkillSymSPtr skiSym = std::dynamic_pointer_cast<SkillSym>(it.second);
            std::string ros2ReqType = skiSym->getROS2ReqType();
            if(!ros2ReqType.empty()) bc.ReqTypeInCPP = cppTypeOf(ros2ReqType);
            bc.className = getClassName(skiSym);
            emitHPP(skiSym); 
            emitCPP(skiSym);
            bc.className.clear();
            bc.ReqTypeInCPP.clear();
        }
    }
    emitBTRunner();
}