open Jest;

module Response = MySql2_response;

describe("MySql2.parseResponse", () => {
  test("Should return an error when given an unexpected boolean.", () => {
    let invalid = Js.Json.boolean(true);
    let message =
      switch (Response.fromDriverResponse(invalid, [||])) {
      | `Select(_) => Failure("unexpected_select_result")
      | `Mutation(_) => Failure("unexpected_mutation_result")
      | `Error(e) => MySql2_error.toExn(e)
      };
    Expect.expect(() =>
      raise(message)
    )
    |> Expect.toThrowMessage("invalid_driver_result");
  });
  test("Should return an error when given an unexpected string", () => {
    let invalid = Js.Json.string("invalid");
    let message =
      switch (Response.fromDriverResponse(invalid, [||])) {
      | `Select(_) => Failure("invalid_select_result")
      | `Mutation(_) => Failure("invalid_mutation_result")
      | `Error(e) => MySql2_error.toExn(e)
      };
    Expect.expect(() =>
      raise(message)
    )
    |> Expect.toThrowMessage("invalid_driver_result");
  });
});
