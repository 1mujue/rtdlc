#include <behaviortree_cpp/action_node.h>
#include <behaviortree_cpp/condition_node.h>
#include <behaviortree_cpp/exceptions.h>
#include <string>
#include <iostream>

class Init : public BT::SyncActionNode {
public:
    Init(const std::string& name, const BT::NodeConfiguration& config)
        : BT::SyncActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("item"),
            BT::InputPort<std::string>("room"),
            BT::OutputPort<bool>("success")
        };
    }

    BT::NodeStatus tick() override {
        // 发送一次topic
        // return val;
    }
};

class CheckExpr : public BT::ConditionNode
{
public:
    CheckExpr(const std::string& name, const BT::NodeConfiguration& config)
        : BT::ConditionNode(name, config)
    {}

    static BT::PortsList providedPorts()
    {
        return {
            BT::InputPort<std::string>("expr")
        };
    }

    BT::NodeStatus tick() override
    {
        auto expr = getInput<std::string>("expr");
        if (!expr)
        {
            throw BT::RuntimeError("CheckExpr: missing input port [expr]");
        }

        // 目前只支持这一种固定表达式
        if (expr.value() != "{local_ok} == true")
        {
            throw BT::RuntimeError("CheckExpr: unsupported expr: ", expr.value());
        }

        auto value = config().blackboard->get<bool>("local_ok");
        return value ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
    }
};