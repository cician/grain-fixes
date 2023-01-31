open Grain_tests.TestFramework;
open Grain_tests.Runner;
open Grain_parsing;
open Grain_parsing.Ast_helper;

describe("parsing", ({test, testSkip}) => {
  let test_or_skip =
    Sys.backend_type == Other("js_of_ocaml") ? testSkip : test;
  let assertParse = makeParseRunner(test);
  let assertFileRun = makeFileRunner(test_or_skip);

  // operators
  assertFileRun(
    "custom_operator",
    "customOperator",
    "Ok(3)\nErr(\"Division by zero!\")\n",
  );
  let a =
    Expression.ident(
      Location.mknoloc(Identifier.IdentName(Location.mknoloc("a"))),
    );
  let b =
    Expression.ident(
      Location.mknoloc(Identifier.IdentName(Location.mknoloc("b"))),
    );
  let c =
    Expression.ident(
      Location.mknoloc(Identifier.IdentName(Location.mknoloc("c"))),
    );
  let testOp = op =>
    assertParse(
      op,
      "module Test; a " ++ op ++ " b",
      {
        module_name: Location.mknoloc("Test"),
        statements: [
          Toplevel.expr(
            Expression.apply(
              Expression.ident(
                Location.mknoloc(
                  Identifier.IdentName(Location.mknoloc(op)),
                ),
              ),
              [a, b],
            ),
          ),
        ],
        comments: [],
        prog_loc: Location.dummy_loc,
      },
    );

  let ops = [
    // Smoketest of operators which should all work
    "||+",
    "||^",
    "&&*&^%",
    "|*",
    "^^^",
    "&-",
    "==!",
    "==$",
    "==*==",
    "!==^",
    "<<<<<",
    "<%>",
    "<=>",
    ">>>>",
    ">>>>>>>>",
    "><><><",
    "+==",
    "+!",
    "++!",
    "+-+",
    "**//**",
    "**",
    // verify that common fan favorites work
    "??",
    "???",
    "+.",
    "-.",
    "*.",
    "/.",
    ">>=",
    "|>",
    ">:",
    "%%",
    "===",
    "!==",
    "==?",
    "&?",
    "++",
    "--",
    "^*^",
    "^-^",
  ];
  List.iter(testOp, ops);

  // verify precedence is maintained
  assertParse(
    "custom_op_precedence_1",
    "module Test; a +++ b *** c",
    {
      module_name: Location.mknoloc("Test"),
      statements: [
        Toplevel.expr(
          Expression.apply(
            Expression.ident(
              Location.mknoloc(
                Identifier.IdentName(Location.mknoloc("+++")),
              ),
            ),
            [
              a,
              Expression.apply(
                Expression.ident(
                  Location.mknoloc(
                    Identifier.IdentName(Location.mknoloc("***")),
                  ),
                ),
                [b, c],
              ),
            ],
          ),
        ),
      ],
      comments: [],
      prog_loc: Location.dummy_loc,
    },
  );
  assertParse(
    "custom_op_precedence_2",
    "module Test; a &&-- b &-- c",
    {
      module_name: Location.mknoloc("Test"),
      statements: [
        Toplevel.expr(
          Expression.apply(
            Expression.ident(
              Location.mknoloc(
                Identifier.IdentName(Location.mknoloc("&&--")),
              ),
            ),
            [
              a,
              Expression.apply(
                Expression.ident(
                  Location.mknoloc(
                    Identifier.IdentName(Location.mknoloc("&--")),
                  ),
                ),
                [b, c],
              ),
            ],
          ),
        ),
      ],
      comments: [],
      prog_loc: Location.dummy_loc,
    },
  );
  assertParse(
    "custom_op_precedence_3",
    "module Test; a ||-- b |-- c",
    {
      module_name: Location.mknoloc("Test"),
      statements: [
        Toplevel.expr(
          Expression.apply(
            Expression.ident(
              Location.mknoloc(
                Identifier.IdentName(Location.mknoloc("||--")),
              ),
            ),
            [
              a,
              Expression.apply(
                Expression.ident(
                  Location.mknoloc(
                    Identifier.IdentName(Location.mknoloc("|--")),
                  ),
                ),
                [b, c],
              ),
            ],
          ),
        ),
      ],
      comments: [],
      prog_loc: Location.dummy_loc,
    },
  );
  assertParse(
    "regression_issue_1473",
    "module Test; a << b >> c",
    {
      module_name: Location.mknoloc("Test"),
      statements: [
        Toplevel.expr(
          Expression.apply(
            Expression.ident(
              Location.mknoloc(
                Identifier.IdentName(Location.mknoloc(">>")),
              ),
            ),
            [
              Expression.apply(
                Expression.ident(
                  Location.mknoloc(
                    Identifier.IdentName(Location.mknoloc("<<")),
                  ),
                ),
                [a, b],
              ),
              c,
            ],
          ),
        ),
      ],
      comments: [],
      prog_loc: Location.dummy_loc,
    },
  );
  assertParse(
    "regression_issue_1609",
    "module Test; return -1",
    {
      module_name: Location.mknoloc("Test"),
      statements: [
        Toplevel.expr(
          Expression.return(
            Some(Expression.constant(PConstNumber(PConstNumberInt("-1")))),
          ),
        ),
      ],
      comments: [],
      prog_loc: Location.dummy_loc,
    },
  );
});
