/* APPLE LOCAL file mainline */
// { dg-options "-W" }

#define NOPE

void foo() {
  while (1); /* { dg-error "suggest a space before " } */
    {
    }
  for (;;); /* { dg-error "suggest a space before " } */
    {
    }
  while (1)
    ;
  for (;;)
    ;
  while (1) ;
  for (;;) ;
  /* These two work when using mapped locations */
  while (1) NOPE; /* { dg-bogus "suggest a space before " "suggest" { xfail *-*-* } } */
  for (;;) NOPE; /* { dg-bogus "suggest a space before " "suggest" { xfail *-*-* } } */
  while (1)
    NOPE;
  for (;;)
    NOPE;
}
