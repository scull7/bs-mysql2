open Jest;

describe("MySql2.parse_response", () => {
  test("Should return an error when given an unexpected boolean.", () => {
    let invalid = Js.Json.boolean(true);
    let message =
      switch (MySql2.parse_response(invalid, [||])) {
      | `Select(_, _) => Failure("invalid_select_result")
      | `Mutation(_, _) => Failure("invalid_mutation_result")
      | `Error(e) => e
      };
    Expect.expect(() =>
      raise(message)
    )
    |> Expect.toThrowMessage("invalid_driver_result");
  });
  test("Should return an error when given an unexpected string", () => {
    let invalid = Js.Json.string("invalid");
    let message =
      switch (MySql2.parse_response(invalid, [||])) {
      | `Select(_, _) => Failure("invalid_select_result")
      | `Mutation(_, _) => Failure("invalid_mutation_result")
      | `Error(e) => e
      };
    Expect.expect(() =>
      raise(message)
    )
    |> Expect.toThrowMessage("invalid_driver_result");
  });
});
