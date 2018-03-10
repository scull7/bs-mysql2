open Jest;

describe("MySql2.parse_response", () => {
  test("Should return an error when given an unexpected boolean.", () => {
    let invalid = Js.Json.boolean(Js.true_);
    let message =
      switch (MySql2.parse_response(invalid, [||])) {
      | `Select(_,_) => "invalid_select_result"
      | `Mutation(_,_) => "invalid_mutation_result"
      | `Error(e) => MySql2.Error.message(e);
      };
    Expect.expect(message) |> Expect.toBe("invalid_driver_result");
  });
  test("Should return an error when given an unexpected string", () => {
    let invalid = Js.Json.string("invalid");
    let message =
      switch (MySql2.parse_response(invalid, [||])) {
      | `Select(_,_) => "invalid_select_result"
      | `Mutation(_,_) => "invalid_mutation_result"
      | `Error(e) => MySql2.Error.message(e);
      };
    Expect.expect(message) |> Expect.toBe("invalid_driver_result");
  });
});
