class ApplicationController < ActionController::Base
protect_from_forgery
before_action :authenticate_user!
  def default_url_options
  { host: ENV["HOST"] || "localhost:3000" }
 end
end
