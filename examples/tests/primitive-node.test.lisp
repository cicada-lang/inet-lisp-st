(define (main)
  (= x x-op (link))
  (= y y-op (link))
  (= z (println (iadd x y)))
  ;; (= z (iadd x y))
  ;; (= z (println z))

  (connect x-op 1)
  (connect y-op 2)
  (run)
  (println z)
  )

(main)
