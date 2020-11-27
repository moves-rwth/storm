#include <storm/exceptions/InvalidArgumentException.h>
#include "test/storm_gtest.h"
#include "storm-config.h"
#include "storm-parsers/parser/JaniParser.h"
#include "storm-parsers/api/model_descriptions.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/ModelType.h"


TEST(JaniParser, DieExampleTest) {
    std::string testInput = R"({
	"jani-version": 1,
	"name": "die.jani",
	"type": "dtmc",
	"features": [ "derived-operators" ],
	"variables": [
		{
			"name": "s",
			"type": {
				"base": "int",
				"kind": "bounded",
				"lower-bound": 0,
				"upper-bound": 7
			},
			"initial-value": 0
		},
		{
			"name": "d",
			"type": {
				"base": "int",
				"kind": "bounded",
				"lower-bound": 0,
				"upper-bound": 6
			},
			"initial-value": 0
		}
	],
	"properties": [
		{
			"name": "Probability to throw a six",
			"expression": {
				"op": "filter",
				"fun": "max",
				"states": { "op": "initial" },
				"values": {
					"op": "Pmin",
					"exp": {
						"op": "U",
						"left": true,
						"right": {
							"op": "∧",
							"left": {
								"op": "=",
								"left": "s",
								"right": 7
							},
							"right": {
								"op": "=",
								"left": "d",
								"right": 6
							}
						}
					}
				}
			}
		},
		{
			"name": "Expected number of coin flips",
			"expression": {
				"op": "filter",
				"fun": "max",
				"states": { "op": "initial" },
				"values": {
					"op": "Emin",
					"accumulate": [ "steps" ],
					"exp": 1,
					"reach": {
						"op": "=",
						"left": "s",
						"right": 7
					}
				}
			}
		}
	],
	"automata": [
		{
			"name": "die",
			"locations": [{ "name": "l" }],
			"initial-locations": ["l"],
			"edges": [
				{
					"location": "l",
					"guard": {
						"exp": {
							"op": "=",
							"left": "s",
							"right": 0
						}
					},
					"destinations": [
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 1
								}
							]
						},
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 2
								}
							]
						}
					]
				},
				{
					"location": "l",
					"guard": {
						"exp": {
							"left": "s",
							"op": "=",
							"right": 1
						}
					},
					"destinations": [
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 3
								}
							]
						},
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 4
								}
							]
						}
					]
				},
				{
					"location": "l",
					"guard": {
						"exp": {
							"left": "s",
							"op": "=",
							"right": 2
						}
					},
					"destinations": [
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 5
								}
							]
						},
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 6
								}
							]
						}
					]
				},
				{
					"location": "l",
					"guard": {
						"exp": {
							"left": "s",
							"op": "=",
							"right": 3
						}
					},
					"destinations": [
						{
							"location": "l",
							"probability": { "exp": 0.5 }, 
							"assignments": [
								{
									"ref": "s",
									"value": 1
								}
							]
						},
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 7
								},
								{
									"ref": "d",
									"value": 1
								}
							]
						}
					]
				},
				{
					"location": "l",
					"guard": {
						"exp": {
							"left": "s",
							"op": "=",
							"right": 4
						}
					},
					"destinations": [
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 7
								},
								{
									"ref": "d",
									"value": 2
								}
							]
						},
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 7
								},
								{
									"ref": "d",
									"value": 3
								}
							]
						}
					]
				},
				{
					"location": "l",
					"guard": {
						"exp": {
							"left": "s",
							"op": "=",
							"right": 5
						}
					},
					"destinations": [
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 7
								},
								{
									"ref": "d",
									"value": 4
								}
							]
						},
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 7
								},
								{
									"ref": "d",
									"value": 5
								}
							]
						}
					]
				},
				{
					"location": "l",
					"guard": {
						"exp": {
							"left": "s",
							"op": "=",
							"right": 6
						}
					},
					"destinations": [
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 2
								}
							]
						},
						{
							"location": "l",
							"probability": { "exp": 0.5 },
							"assignments": [
								{
									"ref": "s",
									"value": 7
								},
								{
									"ref": "d",
									"value": 6
								}
							]
						}
					]
				},
				{
					"location": "l",
					"guard": {
						"exp": {
							"left": "s",
							"op": "=",
							"right": 7
						}
					},
					"destinations": [
						{
							"location": "l",
							"assignments": [
								{
									"ref": "s",
									"value": 7
								}
							]
						}
					]
				}
			]
			
		}
	], 
	"system": {
		"elements": [ { "automaton": "die" } ]
	}
    })";

    std::pair<storm::jani::Model, std::vector<storm::jani::Property>> result;
    EXPECT_NO_THROW(result = storm::api::parseJaniModelFromString(testInput));
    EXPECT_EQ(storm::jani::ModelType::DTMC, result.first.getModelType());
    EXPECT_TRUE(result.first.hasGlobalVariable("s"));
    EXPECT_EQ(1ul, result.first.getNumberOfAutomata());
}

TEST(JaniParser, UnassignedVariablesTest) {
    std::pair<storm::jani::Model, std::vector<storm::jani::Property>> result;
    EXPECT_NO_THROW(result = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/mdp/unassigned-variables.jani"));
    EXPECT_EQ(storm::jani::ModelType::MDP, result.first.getModelType());
    EXPECT_TRUE(result.first.hasConstant("c"));
    EXPECT_EQ(2ul, result.first.getNumberOfAutomata());
}

