#pragma once
#include <string>
#include <array>
#include <set>
#include <map>
#include <list>
#include <optional>
#include <bitset>
#include <vector>


struct BitValue
{
    uint8_t bits;
    size_t size;
};


enum class JTAGState
{
    TLR,
    RTI,
    SELECT_DR_SCAN,
    CAPTURE_DR,
    SHIFT_DR,
    EXIT_1_DR,
    PAUSE_DR,
    EXIT_2_DR,
    UPDATE_DR,
    SELECT_IR_SCAN,
    CAPTURE_IR,
    SHIFT_IR,
    EXIT_1_IR,
    PAUSE_IR,
    EXIT_2_IR,
    UPDATE_IR,
};


class JTAG
{
    private:
        JTAGState _currentState = JTAGState::TLR;

        const std::map<JTAGState, std::array<JTAGState, 2>> _tree = {
            {JTAGState::TLR, {JTAGState::RTI, JTAGState::TLR}},
            {JTAGState::RTI, {JTAGState::RTI, JTAGState::SELECT_DR_SCAN}},

            {JTAGState::SELECT_DR_SCAN, {JTAGState::CAPTURE_DR, JTAGState::SELECT_IR_SCAN}},
            {JTAGState::CAPTURE_DR, {JTAGState::SHIFT_DR, JTAGState::EXIT_1_DR}},
            {JTAGState::SHIFT_DR, {JTAGState::SHIFT_DR, JTAGState::EXIT_1_DR}},
            {JTAGState::EXIT_1_DR, {JTAGState::PAUSE_DR, JTAGState::UPDATE_DR}},
            {JTAGState::PAUSE_DR, {JTAGState::PAUSE_DR, JTAGState::EXIT_2_DR}},
            {JTAGState::EXIT_2_DR, {JTAGState::SHIFT_DR, JTAGState::UPDATE_DR}},
            {JTAGState::UPDATE_DR, {JTAGState::RTI, JTAGState::SELECT_DR_SCAN}},

            {JTAGState::SELECT_IR_SCAN, {JTAGState::CAPTURE_IR, JTAGState::TLR}},
            {JTAGState::CAPTURE_IR, {JTAGState::SHIFT_IR, JTAGState::EXIT_1_IR}},
            {JTAGState::SHIFT_IR, {JTAGState::SHIFT_IR, JTAGState::EXIT_1_IR}},
            {JTAGState::EXIT_1_IR, {JTAGState::PAUSE_IR, JTAGState::UPDATE_IR}},
            {JTAGState::PAUSE_IR, {JTAGState::PAUSE_IR, JTAGState::EXIT_2_IR}},
            {JTAGState::EXIT_2_IR, {JTAGState::SHIFT_IR, JTAGState::UPDATE_IR}},
            {JTAGState::UPDATE_IR, {JTAGState::RTI, JTAGState::SELECT_DR_SCAN}},
        };

        std::optional<std::vector<JTAGState>> _navigate(const JTAGState& ending_position) const
        {
            if (ending_position == _currentState)
                return std::nullopt;

            std::list<std::vector<JTAGState>> queue;

            queue.push_back({_currentState});

            while (!queue.empty()) {
                auto path = queue.front();
                queue.pop_front();
                auto node = path.back();

                if (node == ending_position)
                    return path;

                for(const auto& adjacent : _tree.at(node)) {
                    auto new_path = path;
                    new_path.push_back(adjacent);
                    queue.push_back(new_path);
                }
            }

            return std::nullopt;
        }

        BitValue _recreate_path(const std::vector<JTAGState>& path) const
        {
            uint8_t bit_value = 0;
            size_t count = 0;

            for(size_t i = 0; i < path.size() - 1; i++) {
                JTAGState state = path[i];
                JTAGState nextState = path[i + 1];
                auto tree = _tree.at(state);

                for(size_t p = 0; p < tree.size(); p++) {
                    if(tree.at(p) == nextState) {
                        bit_value <<= 1;
                        bit_value |= p;
                        count++;
                        break;
                    }
                }
            }

            return BitValue{bit_value, count};
        }

    public:
        JTAG() = default;

        std::optional<BitValue> move(JTAGState&& state) {

            if(const auto path = _navigate(state)) {
                _currentState = state;
                return _recreate_path(*path);
            }

            return std::nullopt;
        }

        JTAGState getCurrentState() const {
            return _currentState;
        }
};
