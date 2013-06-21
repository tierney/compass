namespace cpp compass

struct Post {
  1: required string p1,
  2: required string msg,
  3: string p2,
  4: string q,
  5: list<string> expl_attrs
}

struct UserAuth {
  1: string email,
  2: string password,
}

struct Context {
  1: list<string> roles,
  2: list<string> norms,
  3: list<string> emails, # actors
}