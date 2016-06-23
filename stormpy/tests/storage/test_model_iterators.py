import stormpy
from helpers.helper import get_example_path

class TestModelIterators:
    def test_states_dtmc(self):
        model = stormpy.parse_explicit_model(get_example_path("dtmc", "die", "die.tra"), get_example_path("dtmc", "die", "die.lab"))
        s = stormpy.state.State(0, model)
        i = 0
        for state in stormpy.state.State(0, model):
            assert state.id == i
            i += 1
        assert i == model.nr_states()
    
    def test_states_mdp(self):
        model = stormpy.parse_explicit_model(get_example_path("mdp", "two_dice", "two_dice.tra"), get_example_path("mdp", "two_dice", "two_dice.lab"))
        s = stormpy.state.State(0, model)
        i = 0
        for state in stormpy.state.State(0, model):
            assert state.id == i
            i += 1
        assert i == model.nr_states()
    
    def test_actions_dtmc(self):
        model = stormpy.parse_explicit_model(get_example_path("dtmc", "die", "die.tra"), get_example_path("dtmc", "die", "die.lab"))
        s = stormpy.state.State(0, model)
        for state in stormpy.state.State(0, model):
            for action in state.actions():
                assert action.row == 0
    
    def test_actions_mdp(self):
        model = stormpy.parse_explicit_model(get_example_path("mdp", "two_dice", "two_dice.tra"), get_example_path("mdp", "two_dice", "two_dice.lab"))
        s = stormpy.state.State(0, model)
        for state in stormpy.state.State(0, model):
            for action in state.actions():
                assert action.row == 0 or action.row == 1
    
    def test_transitions_dtmc(self):
        transitions_orig = [(0, 0, 0), (0, 1, 0.5), (0, 2, 0.5), (1, 1, 0), (1, 3, 0.5), (1, 4, 0.5), 
                (2, 2, 0), (2, 5, 0.5), (2, 6, 0.5), (3, 1, 0.5), (3, 3, 0), (3, 7, 0.5),
                (4, 4, 0), (4, 8, 0.5), (4, 9, 0.5), (5, 5, 0), (5, 10, 0.5), (5, 11, 0.5),
                (6, 2, 0.5), (6, 6, 0), (6, 12, 0.5), (7, 7, 1), (8, 8, 1),
                (9, 9, 1), (10, 10, 1), (11, 11, 1), (12, 12, 1)
            ]
        model = stormpy.parse_explicit_model(get_example_path("dtmc", "die", "die.tra"), get_example_path("dtmc", "die", "die.lab"))
        s = stormpy.state.State(0, model)
        i = 0
        for state in stormpy.state.State(0, model):
            for action in state.actions():
                for transition in action.transitions():
                    transition_orig = transitions_orig[i]
                    i += 1
                    assert state.id == transition_orig[0]
                    assert transition.column() == transition_orig[1]
                    assert transition.value() == transition_orig[2]
    
    def test_transitions_mdp(self):
        model = stormpy.parse_explicit_model(get_example_path("mdp", "two_dice", "two_dice.tra"), get_example_path("mdp", "two_dice", "two_dice.lab"))
        s = stormpy.state.State(0, model)
        for state in stormpy.state.State(0, model):
            i = 0
            for action in state.actions():
                i += 1
                for transition in action.transitions():
                    assert transition.value() == 0.5 or transition.value() == 1
            assert i == 1 or i == 2

    def test_row_iterator(self):
        transitions_orig = [(0, 0, 0), (0, 1, 0.5), (0, 2, 0.5), (1, 1, 0), (1, 3, 0.5), (1, 4, 0.5),
                (2, 2, 0), (2, 5, 0.5), (2, 6, 0.5), (3, 1, 0.5), (3, 3, 0), (3, 7, 0.5),
                (4, 4, 0), (4, 8, 0.5), (4, 9, 0.5), (5, 5, 0), (5, 10, 0.5), (5, 11, 0.5),
                (6, 2, 0.5), (6, 6, 0), (6, 12, 0.5), (7, 7, 1), (8, 8, 1),
                (9, 9, 1), (10, 10, 1), (11, 11, 1), (12, 12, 1)
            ]
        model = stormpy.parse_explicit_model(get_example_path("dtmc", "die", "die.tra"), get_example_path("dtmc", "die", "die.lab"))
        i = 0
        for state in stormpy.state.State(0, model):
            for transition in model.transition_matrix().row_iter(state.id, state.id):
                transition_orig = transitions_orig[i]
                i += 1
                assert state.id == transition_orig[0]
                assert transition.column() == transition_orig[1]
                assert transition.value() == transition_orig[2]
