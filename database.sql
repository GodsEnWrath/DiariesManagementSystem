CREATE TABLE user_types
(
  id              INT unsigned NOT NULL, 				# Unique ID for the record
  user_type 	  INT unsigned NOT NULL,		        # Type of User, 1=admin, 2=user
  PRIMARY KEY     (id)                                  # Make the id the primary key
);

CREATE TABLE user_base
(
  id              INT unsigned NOT NULL AUTO_INCREMENT, # Unique ID for the record
  user_type 	  INT unsigned NOT NULL,				# Type of User
  username        VARCHAR(16) NOT NULL,					# username
  password        VARCHAR(16) NOT NULL,					# password
  name            VARCHAR(150) NOT NULL,        		# Name 
  date_updated   DATE NOT NULL,	             			# date updated
  date_creation  DATE NOT NULL,	                    	# date creation
  PRIMARY KEY     (id),                        			# Make the id the primary key
);

CREATE TABLE user_diaries
(
  id             INT unsigned NOT NULL AUTO_INCREMENT, # Unique ID for the record
  user_id 	     INT unsigned NOT NULL,		           # Type of User
  diaries_entry  text NOT NULL,		       			   # diary entry
  date_updated   DATE NOT NULL,	                       # date updated
  date_creation  DATE NOT NULL,	                   	   # date creation
  PRIMARY KEY     (id),                                # Make the id the primary key
);