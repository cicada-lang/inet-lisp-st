(define-node null value!)
(define-node cons head tail value!)
(define-node append target! rest result)

(define-rule (append (null) rest result)
  (connect rest result))

(define-rule (append (cons head tail) rest result)
  (cons head (append tail rest) result))

(define-node sole value!)

(define (inspect-run wire)
  (wire-print-net (run (wire-print-net wire))))

(inspect-run
 (append
  (cons (sole) (cons (sole) (cons (sole) (null))))
  (cons (sole) (cons (sole) (cons (sole) (null))))))
