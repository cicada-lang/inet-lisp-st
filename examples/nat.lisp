(define-node zero value!)
(define-node add1 prev value!)
(define-node add target! addend result)

(define-rule (add (zero) addend result)
  (connect addend result))

(define-rule (add (add1 prev) addend result)
  (add1 (add prev addend) result))

(define (two) (add1 (add1 (zero))))

(add (two) (two))

(wire-print-net)
(run)
(wire-print-net)
