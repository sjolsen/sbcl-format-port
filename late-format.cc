#include "late-format.hh"
#include <tuple>
#include <cctype>

std::size_t position (char c, string_t string, std::size_t start)
{
	for (auto pos = start; pos < length (string); ++pos)
		if (schar (string, pos) == c)
			return pos;
	return length (string);
}

std::tuple <int, std::size_t> parse_integer_unsafe (string_t string, std::size_t index)
// Requires that substring(string,index,end(string)) be non-empty and match the
// regex "^[+-]?[0-9]+"
{
	bool negative = false;
	if (schar (string, index) == '-') {
		negative = true;
		++index;
	}
	else if (schar (string, index) == '+')
		++index;

	int result = 0;
	while (index != length (string)
	       and '0' <= schar (string, index)
	       and schar (string, index) <= '9')
	{
		result *= 10;
		result += schar (string, index);
		++index;
	}

	return std::make_tuple (result, index);
}



// (defun parse-directive (string start)
format_directive parse_directive (string_t string, std::size_t start)
{
//   (let ((posn (1+ start)) (params nil) (colonp nil) (atsignp nil)
//         (end (length string)))
	auto posn    = 1 + start;
	auto params  = paramlist {};
	auto colonp  = false;
	auto atsignp = false;
	auto end     = length (string);

//     (flet ((get-char ()
	auto get_char = [&] () {
//              (if (= posn end)
		if (posn == end)
//                  (error 'format-error
//                         :complaint "string ended before directive was found"
//                         :control-string string
//                         :offset start)
			throw format_error ()
				._complaint ("string ended before directive was found")
				._control_string (string)
				._offset (start);
//                  (schar string posn)))
		else
			return schar (string, posn);
	};
//            (check-ordering ()
	auto check_ordering = [&] () {
//              (when (or colonp atsignp)
		if (colonp or atsignp)
//                (error 'format-error
//                       :complaint "parameters found after #\\: or #\\@ modifier"
//                       :control-string string
//                       :offset posn
//                       :references (list '(:ansi-cl :section (22 3)))))))
			throw format_error ()
				._complaint ("parameters found after #\\: or #\\@ modifier")
				._control_string (string)
				._offset (posn)
				._references ("ANSI CL §22.3");
	};
//       (loop
	while (true) {
//         (let ((char (get-char)))
		auto c = get_char ();

//           (cond ((or (char<= #\0 char #\9) (char= char #\+) (char= char #\-))
		if (('0' <= c and c <= '9') or (c == '+') or (c == '-')) {
//                  (check-ordering)
			check_ordering ();
//                  (multiple-value-bind (param new-posn)
//                      (parse-integer string :start posn :junk-allowed t)
			int param;
			std::size_t new_posn;
			std::tie (param, new_posn) = parse_integer_unsafe (string, posn);
//                    (push (cons posn param) params)
			params.push_back (param_t {posn}._int_param (param));
//                    (setf posn new-posn)
			posn = new_posn;
//                    (case (get-char)
			switch (get_char ()) {
//                      (#\,)
				case ',':
					break;
//                      ((#\: #\@)
				case ':':
				case '@':
//                       (decf posn))
					--posn;
					break;
//                      (t
				default:
//                       (return)))))
					goto loop_return;
			}
		}
//                 ((or (char= char #\v)
//                      (char= char #\V))
		else if (c == 'v' or c == 'V') {
//                  (check-ordering)
			check_ordering ();
//                  (push (cons posn :arg) params)
			params.push_back (param_t {posn}._kwd_param (KWD_ARG));
//                  (incf posn)
			++posn;
//                  (case (get-char)
			switch (get_char ()) {
//                    (#\,)
				case ',':
					break;
//                    ((#\: #\@)
				case ':':
				case '@':
//                     (decf posn))
					--posn;
					break;
//                    (t
				default:
//                     (return))))
					goto loop_return;
			}
		}
//                 ((char= char #\#)
		else if (c == '#') {
//                  (check-ordering)
			check_ordering ();
//                  (push (cons posn :remaining) params)
			params.push_back (param_t {posn}._kwd_param (KWD_REMAINING));
//                  (incf posn)
			++posn;
//                  (case (get-char)
			switch (get_char ()) {
//                    (#\,)
				case ',':
					break;
//                    ((#\: #\@)
				case ':':
				case '@':
//                     (decf posn))
					--posn;
					break;
//                    (t
				default:
//                     (return))))
					goto loop_return;
			}
		}
//                 ((char= char #\')
		else if (c == '\'') {
//                  (check-ordering)
			check_ordering ();
//                  (incf posn)
			++posn;
//                  (push (cons posn (get-char)) params)
			params.push_back (param_t {posn}._char_param (get_char ()));
//                  (incf posn)
			++posn;
//                  (unless (char= (get-char) #\,)
			if (get_char () != ',')
//                    (decf posn)))
				--posn;
		}
//                 ((char= char #\,)
		else if (c == ',') {
//                  (check-ordering)
			check_ordering ();
//                  (push (cons posn nil) params))
			params.push_back (param_t {posn}._nil_param ());
		}
//                 ((char= char #\:)
		else if (c == ':') {
//                  (if colonp
			if (colonp)
//                      (error 'format-error
//                             :complaint "too many colons supplied"
//                             :control-string string
//                             :offset posn
//                             :references (list '(:ansi-cl :section (22 3))))
				throw format_error ()
					._complaint ("too many colons supplied")
					._control_string (string)
					._offset (posn)
					._references ("ANSI CL §22.3");
//                      (setf colonp t)))
			else
				colonp = true;
		}
//                 ((char= char #\@)
		else if (c == '@') {
//                  (if atsignp
			if (atsignp)
//                      (error 'format-error
//                             :complaint "too many #\\@ characters supplied"
//                             :control-string string
//                             :offset posn
//                             :references (list '(:ansi-cl :section (22 3))))
				throw format_error ()
					._complaint ("too many #\\@ characters supplied")
					._control_string (string)
					._offset (posn)
					._references ("ANSI CL §22.3");
//                      (setf atsignp t)))
			else
				atsignp = true;
		}
//                 (t
		else {
//                  (when (and (char= (schar string (1- posn)) #\,)
//                             (or (< posn 2)
//                                 (char/= (schar string (- posn 2)) #\')))
			if ((schar (string, posn - 1) == ',') and
			    ((posn < 2) or (schar (string, posn - 2) != '\''))) {
//                    (check-ordering)
				check_ordering ();
//                    (push (cons (1- posn) nil) params))
				params.push_back (param_t {posn - 1}._nil_param ());
			}
//                  (return))))
			goto loop_return;
		}
//         (incf posn))
		++posn;
	}
loop_return:;
//       (let ((char (get-char)))
	auto c = get_char ();
//         (when (char= char #\/)
	if (c == '/') {
//           (let ((closing-slash (position #\/ string :start (1+ posn))))
		auto closing_slash = position ('/', string, posn + 1);
//             (if closing-slash
		if (closing_slash != length (string))
//                 (setf posn closing-slash)
			posn = closing_slash;
//                 (error 'format-error
//                        :complaint "no matching closing slash"
//                        :control-string string
//                        :offset posn))))
		else
			throw format_error ()
				._complaint ("no matching closing slash")
				._control_string (string)
				._offset (posn);
	}
//         (make-format-directive
//          :string string :start start :end (1+ posn)
//          :character (char-upcase char)
//          :colonp colonp :atsignp atsignp
//          :params (nreverse params))))))
	return format_directive {
		string, start, posn + 1,
		static_cast <char> (std::toupper (c)),
		colonp, atsignp,
		std::move (params)
	};
}
