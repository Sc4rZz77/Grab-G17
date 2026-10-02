# Test table (all 9 cases run; results in tests/test_results.txt)

| ID | Type | Keyboard input | Expected | Actual | Pass |
|----|------|----------------|----------|--------|------|
| T1 | Valid | GrabCar, 10 km, normal, no promo | Total RM 18.00 | RM 18.00 | Yes |
| T2 | Valid | GrabBike, 3 km, peak, GRAB10 | Ride 7.05, discount 0.71, total RM 7.34 | Same | Yes |
| T3 | Edge | GrabBike, 0.5 km, normal | Minimum fare 4.00, total RM 5.00 | Same | Yes |
| T4 | Edge | GrabCar Plus, 100 km, late night | Ride 232.50, total RM 233.50 | Same | Yes |
| T5 | Invalid | Service: 9, abc, empty, 2.5, then 2 | 4 error messages, then continues | Same | Yes |
| T6 | Invalid | Distance: abc, -5, 0.2, 150, 1.2.3, then 5 | 5 error messages, then RM 11.50 (GrabCar normal) | Same | Yes |
| T7 | Edge | GrabCar Plus, 100 km, night, grab10 | Discount capped RM 5.00, total RM 228.50 | Same | Yes |
| T8 | Invalid | Promo FREE50 | "not recognised", no discount, total RM 15.40 | Same | Yes |
| T9 | Loop | 'maybe', then y, second trip | Error, then second estimate, then exit | Same | Yes |
