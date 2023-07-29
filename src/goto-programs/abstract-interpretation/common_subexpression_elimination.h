
#pragma once

#include <goto-programs/abstract-interpretation/ai.h>
#include <pointer-analysis/value_set_analysis.h>
/**
 * @brief Abstract domain to obtain all available expressions (AE)
 *
 * The domain is the set of expressions that were already computed up to this point.
 *
 * ... // AE: []
 * int x = a + b + c // AE: [a + b, a + b + c]
 * int y = a + b + d // AE: [a + b, a + b + c, a + b + d]
 * c = 42 // AE: [a + b, a + b + d]
 *
 *
 * There are some level of precision to be considered. We shouldn't track constants
 * or a symbols as there is no point in saying that something like '42' is available.
 * However, '42 + a' will be cached. This opens a path for canonization algorithms as
 * '42 + a' = 'a + 42'
 *
 * TODO: add a canonization method for expr2tc.
 *
 * In Summary, the abstract domain is implemented as:
 * - TOP --> all expressions possible for the program (not needed)
 * - BOTTOM --> no expressions are available
 * - Meet operator: AE1 `meet` AE2 = AE2
 * - Transform operator:
 *   + END_FUNCTION: all local variables are not available anymore
 *   + ASSIGN: RHS (and sub-expressions) is now available, LHS (and dependencies) is not available anymore
 *   + GOTO/ASSERT/ASSUME: guard (and sub-expressions) is now available
 *   + DECL/DEAD: variable is no longer available
 *   + FUNCTION_CALL: same as assign
 */

class cse_domaint : public ai_domain_baset
{
public:
  cse_domaint() = default;

  virtual void transform(
    goto_programt::const_targett from,
    goto_programt::const_targett to,
    ai_baset &ai,
    const namespacet &ns) override;

  virtual void output(std::ostream &out) const override;

  virtual void make_bottom() override
  {
    // A bottom for AE means that there are no expressions available
    available_expressions.clear();
  }

  virtual void make_entry() override
  {
    available_expressions.clear();
  };
  virtual void make_top() override{
    // Theoretically there exists a TOP (all possible AE in the program).
    // In practice, there is no need for it.
    throw "[CSE] Available Expressions does not implement make_top()";
  };

  virtual bool is_bottom() const override
  {
    return available_expressions.size() == 0;
  }
  virtual bool is_top() const override
  {
    // Not needed
    return false;
  }

  virtual bool ai_simplify(expr2tc &, const namespacet &) const override
  {
    return false;
  }

protected:
  bool join(const cse_domaint &b);

public:
  bool merge(
    const cse_domaint &b,
    goto_programt::const_targett,
    goto_programt::const_targett)
  {
    // We don't care about the type of instruction to merge.
    return join(b);
  }


  /// All expressions available
  std::unordered_set<expr2tc, irep2_hash> available_expressions;

protected:

  /// Add non-primitive expression `e` (and its sub-expressions) into available_expressions.
  void make_expression_available(const expr2tc &e);

  /// Remove expression `e` (and everything that it depends on) from available_expressions
  void havoc_expr(const expr2tc &e, const goto_programt::const_targett &);

  /// Remove every expression from available_expressions that depends on symbol `sym`
  void havoc_symbol(const irep_idt &sym);

  // Helper function to check whether `src` depends on `taint`
  bool should_remove_expr(const expr2tc &taint, const expr2tc &src) const;
    // Helper function to check whether `src` depends on symbol `sym`
  bool should_remove_expr(const irep_idt &sym, const expr2tc &src) const;


public:
  // TODO: clearly this shouldn't be here. The proper way is to create a new Abstract Interpreter
  // that contains a points-to analysis
  static std::unique_ptr<value_set_analysist> vsa;
};

#include <util/algorithms.h>
/**
 * Common Subexpression Elimination algorithm
 */
class goto_cse : public goto_functions_algorithm
{
public:
  explicit goto_cse(const namespacet &ns)
    : goto_functions_algorithm(true), ns(ns)
  {
  }

  virtual bool runOnProgram(goto_functionst &) override;
  virtual bool
  runOnFunction(std::pair<const dstring, goto_functiont> &F) override;

  unsigned threshold = 1;
  bool verbose_mode = true;

protected:
  ait<cse_domaint> available_expressions;
  const namespacet &ns;
  expr2tc obtain_max_sub_expr(const expr2tc &e, const cse_domaint &state) const;
  void replace_max_sub_expr(
    expr2tc &e,
    std::unordered_map<expr2tc, expr2tc, irep2_hash> &expr2symbol) const;

  symbol2tc create_cse_symbol(const type2tc &t);

private:
  unsigned symbol_counter = 0;
  const std::string prefix = "__esbmc_cse_symbol";
};